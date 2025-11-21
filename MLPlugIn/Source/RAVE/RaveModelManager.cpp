/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#include "RaveModelManager.h"
#include <cstring>
#include <iostream>

void RaveModelManager::loadAsync(const std::string &path) {
    std::thread([this, path]() {
        try {
            std::cout << "[RAVE] Loading model: " << path << std::endl;

            auto model = std::make_shared<torch::jit::script::Module>(
                torch::jit::load(path));

            // Store shared_ptr atomically (RT-safe read)
            auto* heapPtr = new std::shared_ptr<torch::jit::script::Module>(model);
            auto old = modelPtrAtomic.exchange(reinterpret_cast<std::uintptr_t>(heapPtr), std::memory_order_release);
            
            
            if (old != 0)
                delete reinterpret_cast<std::shared_ptr<torch::jit::script::Module>*>(old);
            modelLoaded.store(true, std::memory_order_release);

            std::cout << "[RAVE] Model loaded, starting worker thread."
                      << std::endl;

            // Start worker thread AFTER model is loaded
            workerThread =
                std::thread(&RaveModelManager::workerThreadFunc, this);

        } catch (const std::exception &e) {
            std::cerr << "[RAVE] Failed loading model: " << e.what()
                      << std::endl;
        }
    }).detach();
}

void RaveModelManager::pushInputSample(float left, float right,
                                       bool &readyForOutput) {
    readyForOutput = false;

    currentInputBlock.samplesL[writePos] = left;
    currentInputBlock.samplesR[writePos] = right;
    writePos++;

    if (writePos >= RAVE_BLOCK_SIZE) {
        currentInputBlock.seq = nextInputSeq++;

        // Try to push block to inference thread
        if (!inputFIFO.push(currentInputBlock)) {
            // FIFO full → drop block (realtime safe)
            // No logging in RT thread
        }

        writePos = 0;
        readyForOutput = true;
    }
}

bool RaveModelManager::getProcessedSample(float &left, float &right) {
    static AudioBlock currentOutputBlock;
    static int readPos = RAVE_BLOCK_SIZE; // empty at start

    // Need new block?
    if (readPos >= RAVE_BLOCK_SIZE) {
        if (!outputFIFO.pop(currentOutputBlock)) {
            // No processed block yet → fail (will fallback to dry audio)
            return false;
        }
        readPos = 0;
        nextOutputSeq = currentOutputBlock.seq;
    }

    // Consume one sample
    left = currentOutputBlock.samplesL[readPos];
    right = currentOutputBlock.samplesR[readPos];
    readPos++;

    return true;
}

void RaveModelManager::workerThreadFunc() {
    std::cout << "[RAVE] Worker thread started." << std::endl;

    while (keepRunning.load(std::memory_order_acquire)) {
        // Wait until model is loaded
        if (!modelLoaded.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Load model pointer from atomic raw pointer
        auto rawPtr = reinterpret_cast<std::shared_ptr<torch::jit::script::Module>*>(
            modelPtrAtomic.load(std::memory_order_acquire));
        if (!rawPtr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        auto model = *rawPtr;
        if (!model)
            continue;

        // Pop input block from FIFO
        AudioBlock inBlock;
        if (!inputFIFO.pop(inBlock)) {
            std::this_thread::sleep_for(std::chrono::microseconds(200));
            continue;
        }

        // ---- Convert block → tensor ----
        torch::NoGradGuard noGrad;
        torch::Tensor input = torch::zeros({1, 1, RAVE_BLOCK_SIZE}, torch::kFloat32);
        memcpy(input[0][0].data_ptr<float>(), inBlock.samplesL.data(),
               RAVE_BLOCK_SIZE * sizeof(float));

        // ---- Inference ----
        torch::Tensor outputTensor;
        try {
            auto out = model->forward({input});
            outputTensor = out.toTensor();
        } catch (...) {
            std::cerr << "[RAVE] Model forward() FAILED.\n";
            continue;
        }

        // ---- Tensor → block ----
        AudioBlock outBlock;
        outBlock.seq = inBlock.seq;

        auto cpuTensor = outputTensor.to(torch::kCPU);
        auto ptr = cpuTensor[0][0].data_ptr<float>();
        memcpy(outBlock.samplesL.data(), ptr, RAVE_BLOCK_SIZE * sizeof(float));
        memset(outBlock.samplesR.data(), 0, RAVE_BLOCK_SIZE * sizeof(float)); // R-mono

        // ---- Push result into output FIFO ----
        if (!outputFIFO.push(outBlock)) {
            // FIFO full → drop block (real-time safe)
        }
    }

    std::cout << "[RAVE] Worker thread ended." << std::endl;
}

std::shared_ptr<torch::jit::script::Module> RaveModelManager::getModel() const {
    auto rawPtr = reinterpret_cast<std::shared_ptr<torch::jit::script::Module>*>(
        modelPtrAtomic.load(std::memory_order_acquire));
    return rawPtr ? *rawPtr : nullptr;
}

bool RaveModelManager::isLoaded() const {
    return atomicPtr.load(std::memory_order_acquire) != 0;
}

void RaveModelManager::resetFIFOs() {
    inputFIFO.clear();
    outputFIFO.clear();
    writePos = 0;
    nextInputSeq = 0;
    nextOutputSeq = 0;
    
}

RaveModelManager::~RaveModelManager() {
    keepRunning.store(false, std::memory_order_release);
    if (workerThread.joinable())
        workerThread.join();

    // Clean up model pointer
    auto raw = reinterpret_cast<std::shared_ptr<torch::jit::script::Module>*>(
        modelPtrAtomic.load());
    if (raw)
        delete raw;
}
