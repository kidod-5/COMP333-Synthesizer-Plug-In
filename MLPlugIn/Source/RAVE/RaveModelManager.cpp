/*
  ==============================================================================

    RAVE model class

  ==============================================================================
*/

#include "RaveModelManager.h"
#include <iostream>

void RaveModelManager::loadAsync(const std::string& path)
{
    std::thread([this, path]()
    {
        try {
            std::cout << "[RAVE] Loading model: " << path << std::endl;

            auto model = std::make_shared<torch::jit::script::Module>(
                torch::jit::load(path)
            );

            // Leak-proof ownership handoff:
            // We allocate a NEW shared_ptr on the heap
            auto* heapPtr = new std::shared_ptr<torch::jit::script::Module>(model);

            // Atomically swap in pointer bits
            atomicPtr.store(reinterpret_cast<std::uintptr_t>(heapPtr),
                            std::memory_order_release);

            std::cout << "[RAVE] Model loaded.\n";

        } catch (const std::exception& e) {
            std::cerr << "[RAVE] Failed loading model: " << e.what() << "\n";
        }
    }).detach();
}

std::shared_ptr<torch::jit::script::Module> RaveModelManager::getModel() const
{
    auto raw = reinterpret_cast<std::shared_ptr<torch::jit::script::Module>*>(
        atomicPtr.load(std::memory_order_acquire)
    );

    if (!raw)
        return nullptr;

    return *raw;   // copy shared_ptr → safe for audio thread
}

bool RaveModelManager::isLoaded() const
{
    return atomicPtr.load(std::memory_order_acquire) != 0;
}
