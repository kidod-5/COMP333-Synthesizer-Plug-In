/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#pragma once
#include <torch/script.h>
#include <memory>
#include <thread>
#include <atomic>

class RaveModelManager
{
public:
    RaveModelManager() = default;
    ~RaveModelManager() = default;

    void loadAsync(const std::string& path);

    // Realtime-safe accessor
    std::shared_ptr<torch::jit::script::Module> getModel() const;

    bool isLoaded() const;

private:
    std::atomic<std::uintptr_t> atomicPtr { 0 };  // stores raw pointer bits
};
