/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#include "RaveModelManager.h"
#include <cstring>
#include <iostream>

bool RaveModelManager::loadModel(const std::string &path) {
    std::thread([this, path]() {
        try {
            std::cout << "[RAVE] Loading model: " << path << std::endl;

            auto newModel = std::make_shared<torch::jit::script::Module>(
                torch::jit::load(path));

            newModel->eval();

            //            // ******************************

            //            for (const auto &sub : model->named_children()) {
            //                std::cout << "Child module: " << sub.name <<
            //                std::endl;
            //            }
            //            for (const auto &attr : model->named_attributes()) {
            //                std::cout << "Attribute: " << attr.name <<
            //                std::endl;
            //            }

            //            // ******************************

            std::atomic_store_explicit(&model, newModel,
                                       std::memory_order_release);
            resetFIFOs();
            std::cout << "[RAVE] Model loaded succesfully" << std::endl;

        } catch (const std::exception &e) {
            std::cerr << "[RAVE] Failed loading model: " << e.what()
                      << std::endl;
        }
    }).detach();
    return true;
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

        auto localModel =
            std::atomic_load_explicit(&model, std::memory_order_acquire);
        if (!localModel) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // Pop input block from FIFO
        AudioBlock inBlock;
        if (!inputFIFO.pop(inBlock)) {
            std::this_thread::sleep_for(std::chrono::microseconds(200));
            continue;
        }

        //        torch::Tensor inputTensor = audioBlockToTensor(inBlock);
        //
        //        torch::Tensor z = encode(inputTensor, model);
        //        if (!z.defined() || z.numel() == 0) {
        //            std::cerr << "[RAVE] encode() returned undefined or empty
        //            tensor; skipping block.\n"; continue;
        //        }
        //
        //        modifyLatent(z);
        //
        //        torch::Tensor outputTensor = decode(z, model);
        //        if (!outputTensor.defined() || outputTensor.numel() == 0) {
        //            std::cerr << "[RAVE] decode() returned undefined or empty
        //            tensor; skipping block.\n"; continue;
        //        }
        //
        //        AudioBlock outBlock = tensorToAudioBlock(outputTensor,
        //        inBlock.seq); outputFIFO.push(outBlock);

        // ---- Convert block → tensor ----
        torch::NoGradGuard noGrad;
        torch::Tensor input =
            torch::zeros({1, 1, RAVE_BLOCK_SIZE}, torch::kFloat32);
        memcpy(input[0][0].data_ptr<float>(), inBlock.samplesL.data(),
               RAVE_BLOCK_SIZE * sizeof(float));

        // ---- Inference ----
        torch::Tensor outputTensor;
        try {
            auto out = localModel->forward({input});
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
        memcpy(outBlock.samplesR.data(), ptr,
               RAVE_BLOCK_SIZE * sizeof(float)); // duplicate L → R

        // ---- Push result into output FIFO ----
        if (!outputFIFO.push(outBlock)) {
            // FIFO full → drop block (real-time safe)
        }
    }

    std::cout << "[RAVE] Worker thread ended." << std::endl;
}

std::shared_ptr<torch::jit::script::Module> RaveModelManager::getModel() const {
    return std::atomic_load(&model);
}

void RaveModelManager::resetFIFOs() {
    inputFIFO.clear();
    outputFIFO.clear();
    writePos = 0;
    nextInputSeq = 0;
    nextOutputSeq = 0;
}

//bool RaveModelManager::isLoaded() const {
//    return std::atomic_load(&model) != nullptr;
//}

//torch::Tensor RaveModelManager::audioBlockToTensor(const AudioBlock &block) {
//    int CHANNELS = 16;
//    torch::Tensor input =
//        torch::zeros({1, CHANNELS, RAVE_BLOCK_SIZE}, torch::kFloat32);
//
//    for (int c = 0; c < CHANNELS; ++c) {
//        memcpy(input[0][c].data_ptr<float>(), block.samplesL.data(),
//               RAVE_BLOCK_SIZE * sizeof(float));
//    }
//
//    return input;
//}
//
//RaveModelManager::encode(const torch::Tensor &input,
//                         std::shared_ptr<torch::jit::script::Module> model) {
//    // Calls model->run_method("encode", input) and returns a Tensor.
//    // Safe extraction if the returned IValue is a tensor, tuple, or list.
//
//}
//
//void RaveModelManager::modifyLatent(torch::Tensor &z) {
//    // Example: add 1.0 to channel 3
//    // Guard against out-of-range indexing
//    //    if (!z.defined()) return;
//    //    if (z.dim() >= 2 && z.size(1) > 3) {
//    //        z[0][3] += 1.0f;
//    //    }
//    return;
//}
//
//RaveModelManager::decode(const torch::Tensor &z,
//                         std::shared_ptr<torch::jit::script::Module> model) {
//    // We expect `z` in shape [1, latent_channels, latent_time].
//    // Decoder expects channels/time swapped, so transpose before calling
//    // decode.
//    return;
//}
//
//AudioBlock RaveModelManager::tensorToAudioBlock(const torch::Tensor &tensor,
//                                                uint64_t seq) {
//    AudioBlock outBlock;
//    outBlock.seq = seq;
//
//    auto cpuTensor = tensor.to(torch::kCPU);
//    auto ptr = cpuTensor[0][0].data_ptr<float>();
//    memcpy(outBlock.samplesL.data(), ptr, RAVE_BLOCK_SIZE * sizeof(float));
//    memcpy(outBlock.samplesR.data(), ptr,
//           RAVE_BLOCK_SIZE * sizeof(float)); // duplicate L → R
//
//    return outBlock;
//}

RaveModelManager::RaveModelManager() {
    workerThread = std::thread(&RaveModelManager::workerThreadFunc, this);
}

RaveModelManager::~RaveModelManager() {
    keepRunning.store(false, std::memory_order_release);

    if (workerThread.joinable())
        workerThread.join();

    std::atomic_store_explicit(&model,
                               std::shared_ptr<torch::jit::script::Module>{},
                               std::memory_order_release);
}
