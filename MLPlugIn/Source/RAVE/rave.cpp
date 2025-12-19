/*
  ==============================================================================

    Load torchscript PyTorch pre-trained model and display some parameters

  ==============================================================================
*/

#include "rave.h"
#include <iostream>
#include <memory>
#include <torch/script.h>
#include <torch/torch.h>

void printModelInfo(const torch::jit::script::Module &model,
                    int max_layers = 5) {
    std::cout << "=== Model Info ===" << std::endl;

    int count = 0;
    size_t total_params = 0;

    for (const auto &param_pair : model.named_parameters()) {
        const std::string &name = param_pair.name;
        const at::Tensor &tensor = param_pair.value;

        // Count total parameters
        total_params += tensor.numel();

        if (count < max_layers) {
            std::cout << "Parameter name: " << name << std::endl;
            std::cout << "Parameter shape: [";
            for (size_t i = 0; i < tensor.sizes().size(); ++i) {
                std::cout << tensor.size(i);
                if (i + 1 < tensor.sizes().size())
                    std::cout << ", ";
            }
            std::cout << "]" << std::endl;

            auto flat = tensor.flatten();
            std::cout << "First few values: ";
            for (int j = 0; j < std::min<int>(5, flat.size(0)); ++j)
                std::cout << flat[j].item<float>() << " ";
            std::cout << std::endl << "-----------------" << std::endl;
        }

        ++count;
    }

    // Print total parameters and approximate size
    double sizeMB = total_params * sizeof(float) / (1024.0 * 1024.0);
    std::cout << "Total number of parameters: " << total_params << std::endl;
    std::cout << "Approximate model size in memory: " << sizeMB << " MB"
              << std::endl;
}

torch::jit::script::Module
loadTorchScriptModelAsync(const std::string &model_path) {
    torch::jit::script::Module model;

    try {
        model = torch::jit::load(model_path);
        std::cout << "Model loaded successfully from: " << model_path
                  << std::endl;
        // printModelInfo(model);
    } catch (const c10::Error &e) {
        std::cerr << "Error loading model from: " << model_path << std::endl;
        throw;
    }
    return model;
}

torch::Tensor audioBufferToTensor(const juce::AudioBuffer<float> &buffer) {
    // RAVE V1 expects: 1 batch, 1 channel, 2048 samples
    constexpr int raveBlock = 2048;

    torch::Tensor inputTensor =
        torch::zeros({1, 1, raveBlock}, torch::kFloat32);

    const int numSamples = buffer.getNumSamples();
    const int copyAmount = std::min(numSamples, raveBlock);

    // Always use left channel for mono encoding
    const float *in = buffer.getReadPointer(0);

    std::memcpy(inputTensor[0][0].data_ptr<float>(), in,
                copyAmount * sizeof(float));

    return inputTensor;
}

void tensorToAudioBuffer(const torch::Tensor &tensor,
                         juce::AudioBuffer<float> &buffer) {
    const int numSamples = buffer.getNumSamples();

    // tensor shape == [1, 1, 2048]
    auto cpu = tensor.to(torch::kCPU);

    const float *modelOut = cpu[0][0].data_ptr<float>();
    float *outLeft = buffer.getWritePointer(0);

    std::memcpy(outLeft, modelOut, numSamples * sizeof(float));

    // RAVE V1 is mono → silence right channel
    if (buffer.getNumChannels() > 1)
        buffer.clear(1, 0, numSamples);
}
