/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#include "RaveModelManager.h"
#include <cstring>
#include <iostream>

bool RaveModelManager::loadAsync(const std::string &path) {
    std::thread([this, path]() {
        try {
            std::cout << "[RAVE] Loading model: " << path << std::endl;

            auto model = std::make_shared<torch::jit::script::Module>(
                torch::jit::load(path));

            model->eval();
            
//            // ******************************
            
//            for (const auto &sub : model->named_children()) {
//                std::cout << "Child module: " << sub.name << std::endl;
//            }
//            for (const auto &attr : model->named_attributes()) {
//                std::cout << "Attribute: " << attr.name << std::endl;
//            }
            
            
//            // ******************************
            

            // Store shared_ptr atomically (RT-safe read)
            auto *heapPtr =
                new std::shared_ptr<torch::jit::script::Module>(model);
            auto old = modelPtrAtomic.exchange(
                reinterpret_cast<std::uintptr_t>(heapPtr),
                std::memory_order_release);

            if (old != 0)
                delete reinterpret_cast<
                    std::shared_ptr<torch::jit::script::Module> *>(old);
            modelLoaded.store(true, std::memory_order_release);

            std::cout << "[RAVE] Model loaded, starting worker thread."
                      << std::endl;

            // Start worker thread AFTER model is loaded
            workerThread =
                std::thread(&RaveModelManager::workerThreadFunc, this);

            return true;

        } catch (const std::exception &e) {
            std::cerr << "[RAVE] Failed loading model: " << e.what()
                      << std::endl;
            return false;
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
        auto rawPtr =
            reinterpret_cast<std::shared_ptr<torch::jit::script::Module> *>(
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

//        torch::Tensor inputTensor = audioBlockToTensor(inBlock);
//        
//        torch::Tensor z = encode(inputTensor, model);
//        if (!z.defined() || z.numel() == 0) {
//            std::cerr << "[RAVE] encode() returned undefined or empty tensor; skipping block.\n";
//            continue;
//        }
//        
//        modifyLatent(z);
//        
//        torch::Tensor outputTensor = decode(z, model);
//        if (!outputTensor.defined() || outputTensor.numel() == 0) {
//            std::cerr << "[RAVE] decode() returned undefined or empty tensor; skipping block.\n";
//            continue;
//        }
//        
//        AudioBlock outBlock = tensorToAudioBlock(outputTensor, inBlock.seq);
//        outputFIFO.push(outBlock);

                // ---- Convert block → tensor ----
                torch::NoGradGuard noGrad;
                torch::Tensor input = torch::zeros({1, 1, RAVE_BLOCK_SIZE},
                torch::kFloat32); memcpy(input[0][0].data_ptr<float>(),
                inBlock.samplesL.data(),
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
                memcpy(outBlock.samplesL.data(), ptr, RAVE_BLOCK_SIZE *
                sizeof(float)); memcpy(outBlock.samplesR.data(), ptr,
                RAVE_BLOCK_SIZE * sizeof(float)); // duplicate L → R
        
                // ---- Push result into output FIFO ----
                if (!outputFIFO.push(outBlock)) {
                    // FIFO full → drop block (real-time safe)
                }
    }

    std::cout << "[RAVE] Worker thread ended." << std::endl;
}

std::shared_ptr<torch::jit::script::Module> RaveModelManager::getModel() const {
    auto rawPtr =
        reinterpret_cast<std::shared_ptr<torch::jit::script::Module> *>(
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

torch::Tensor RaveModelManager::audioBlockToTensor(const AudioBlock &block) {
    int CHANNELS = 16;
    torch::Tensor input =
        torch::zeros({1, CHANNELS, RAVE_BLOCK_SIZE}, torch::kFloat32);

    for (int c = 0; c < CHANNELS; ++c) {
        memcpy(input[0][c].data_ptr<float>(), block.samplesL.data(),
               RAVE_BLOCK_SIZE * sizeof(float));
    }
    
    return input;
}

torch::Tensor RaveModelManager::encode(
    const torch::Tensor &input,
    std::shared_ptr<torch::jit::script::Module> model) {
    // Calls model->run_method("encode", input) and returns a Tensor.
    // Safe extraction if the returned IValue is a tensor, tuple, or list.
    torch::NoGradGuard noGrad;

    try {
        torch::jit::IValue iv = model->run_method("encode", input);

        // Direct tensor return
        if (iv.isTensor()) {
            return iv.toTensor();
        }

        // Tuple-like return: take first element
        if (iv.isTuple()) {
            auto tup = iv.toTuple();
            if (tup->elements().size() > 0 && tup->elements()[0].isTensor()) {
                return tup->elements()[0].toTensor();
            }
        }

        // List-like return: try to grab index 0
        if (iv.isList()) {
            auto list = iv.toList();
            if (list.size() > 0 && list.get(0).isTensor()) {
                return list.get(0).toTensor();
            }
        }

        // As a last resort, try to convert to tensor (will throw if impossible)
        try {
            return iv.toTensor();
        } catch (...) {
            std::cerr << "[RAVE] encode(): returned IValue is not a tensor/tuple/list-of-tensor.\n";
            return torch::Tensor(); // undefined
        }

    } catch (const std::exception &e) {
        std::cerr << "[RAVE] encode() exception: " << e.what() << std::endl;
        return torch::Tensor(); // undefined
    } catch (...) {
        std::cerr << "[RAVE] encode() unknown exception.\n";
        return torch::Tensor();
    }
}

//torch::Tensor RaveModelManager::encode(const torch::Tensor& input, std::shared_ptr<torch::jit::script::Module> model) {
//    torch::NoGradGuard noGrad;
//    torch::jit::Module encoder = model->attr("encoder").toModule();
//    return encoder.forward({input}).toTensor(); // [1, latent_channels, latent_time]
//}

void RaveModelManager::modifyLatent(torch::Tensor &z) {
    // Example: add 1.0 to channel 3
    // Guard against out-of-range indexing
//    if (!z.defined()) return;
//    if (z.dim() >= 2 && z.size(1) > 3) {
//        z[0][3] += 1.0f;
//    }
    return;
    // In a real plugin, you could apply sliders or modulators here
}


torch::Tensor RaveModelManager::decode(
    const torch::Tensor &z,
    std::shared_ptr<torch::jit::script::Module> model) {
    // We expect `z` in shape [1, latent_channels, latent_time].
    // Decoder expects channels/time swapped, so transpose before calling decode.
    torch::NoGradGuard noGrad;

    if (!z.defined()) {
        std::cerr << "[RAVE] decode(): input latent is undefined.\n";
        return torch::Tensor();
    }

    torch::Tensor z_dec;
    try {
        z_dec = z.transpose(1, 2); // swap channels <-> time
    } catch (...) {
        std::cerr << "[RAVE] decode(): transpose failed.\n";
        return torch::Tensor();
    }

    try {
        torch::jit::IValue iv = model->run_method("decode", z_dec);

        if (iv.isTensor()) {
            return iv.toTensor();
        }

        if (iv.isTuple()) {
            auto tup = iv.toTuple();
            if (tup->elements().size() > 0 && tup->elements()[0].isTensor()) {
                return tup->elements()[0].toTensor();
            }
        }

        if (iv.isList()) {
            auto list = iv.toList();
            if (list.size() > 0 && list.get(0).isTensor()) {
                return list.get(0).toTensor();
            }
        }

        try {
            return iv.toTensor();
        } catch (...) {
            std::cerr << "[RAVE] decode(): returned IValue is not a tensor/tuple/list-of-tensor.\n";
            return torch::Tensor();
        }

    } catch (const std::exception &e) {
        std::cerr << "[RAVE] decode() exception: " << e.what() << std::endl;
        return torch::Tensor();
    } catch (...) {
        std::cerr << "[RAVE] decode() unknown exception.\n";
        return torch::Tensor();
    }
}

//torch::Tensor RaveModelManager::decode(const torch::Tensor& z, std::shared_ptr<torch::jit::script::Module> model) {
//    torch::NoGradGuard noGrad;
//    torch::Tensor z_dec = z.transpose(1, 2); // swap channels <-> time
//    torch::jit::Module decoder = model->attr("decoder").toModule();
//    return decoder.forward({z_dec}).toTensor(); // [1, channels, time]
//}

AudioBlock RaveModelManager::tensorToAudioBlock(const torch::Tensor& tensor, uint64_t seq) {
    AudioBlock outBlock;
    outBlock.seq = seq;

    auto cpuTensor = tensor.to(torch::kCPU);
    auto ptr = cpuTensor[0][0].data_ptr<float>();
    memcpy(outBlock.samplesL.data(), ptr, RAVE_BLOCK_SIZE * sizeof(float));
    memcpy(outBlock.samplesR.data(), ptr, RAVE_BLOCK_SIZE * sizeof(float)); // duplicate L → R

    return outBlock;
}

RaveModelManager::~RaveModelManager() {
    keepRunning.store(false, std::memory_order_release);
    if (workerThread.joinable())
        workerThread.join();

    // Clean up model pointer
    auto raw = reinterpret_cast<std::shared_ptr<torch::jit::script::Module> *>(
        modelPtrAtomic.load());
    if (raw)
        delete raw;
}
