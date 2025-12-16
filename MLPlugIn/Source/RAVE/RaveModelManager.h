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
    RaveModelManager();// = default;
    ~RaveModelManager();

    bool loadModel(const std::string &path);
    
    void pushInputSample(float left, float right, bool &readyForOutput);
    
    bool getProcessedSample(float &left, float &right);

    std::shared_ptr<torch::jit::script::Module> getModel() const;
    
    bool isLoaded() const;
    
    void resetFIFOs();
    
//    torch::Tensor encode(const torch::Tensor& audio);
//    
//    torch::Tensor decode(const torch::Tensor& latent);
//    
//    void tweakLatent(torch::Tensor& latent, int channel, float value);
    

  private:
    void workerThreadFunc();
    torch::Tensor audioBlockToTensor(const AudioBlock& block);
    torch::Tensor encode(const torch::Tensor& input, std::shared_ptr<torch::jit::script::Module> model);
    void modifyLatent(torch::Tensor& z);
    torch::Tensor decode(const torch::Tensor& z, std::shared_ptr<torch::jit::script::Module> model);
    AudioBlock tensorToAudioBlock(const torch::Tensor& tensor, uint64_t seq);


    // model pointer
    //std::atomic<std::uintptr_t> modelPtrAtomic{0};
    //std::atomic<bool> modelLoaded{false};

    // inference thread
    std::atomic<bool> keepRunning{true};
    std::shared_ptr<torch::jit::script::Module> model;
    std::thread workerThread;

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
