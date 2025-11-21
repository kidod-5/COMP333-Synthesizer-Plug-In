/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#pragma once
#include "BlockTypes.h"
#include "FIFO.h"
#include <atomic>
#include <memory>
#include <thread>
#include <torch/script.h>

class RaveModelManager {
  public:
    RaveModelManager() = default;
    ~RaveModelManager();

    void loadAsync(const std::string &path);
    void pushInputSample(float left, float right, bool &readyForOutput);
    bool getProcessedSample(float &left, float &right);
    std::shared_ptr<torch::jit::script::Module> getModel() const;
    bool isLoaded() const;
    
    void resetFIFOs();

  private:
    std::atomic<std::uintptr_t> atomicPtr{0}; // stores raw pointer bits
    void workerThreadFunc();

    // model pointer
    std::atomic<std::uintptr_t> modelPtrAtomic{0};
    std::atomic<bool> modelLoaded{false};

    // inference thread
    std::thread workerThread;
    std::atomic<bool> keepRunning{true};

    // seq counters
    uint64_t nextInputSeq = 0;
    uint64_t nextOutputSeq = 0;

    // block accumulation
    int writePos = 0;
    AudioBlock currentInputBlock;

    // FIFOs
    LockFreeFIFO<AudioBlock, 8> inputFIFO;
    LockFreeFIFO<AudioBlock, 8> outputFIFO;
};
