/*
  ==============================================================================

    Load torchscript PyTorch pre-trained model and display some parameters

  ==============================================================================
*/

#include "rave.h"
#include <torch/torch.h>
#include <torch/script.h>
#include <iostream>
#include <memory>

void printModelInfo(const torch::jit::script::Module& model, int max_layers = 5) {
    std::cout << "=== Model Info ===" << std::endl;

    int count = 0;
    for (const auto& param_pair : model.named_parameters()) {
        if (count >= max_layers) break;

        const std::string& name = param_pair.name;
        const at::Tensor& tensor = param_pair.value;

        std::cout << "Parameter name: " << name << std::endl;
        std::cout << "Parameter shape: [";
        for (size_t i = 0; i < tensor.sizes().size(); ++i) {
            std::cout << tensor.size(i);
            if (i + 1 < tensor.sizes().size()) std::cout << ", ";
        }
        std::cout << "]" << std::endl;

        // Optional: print the first few values
        auto flat = tensor.flatten();
        std::cout << "First few values: ";
        for (int j = 0; j < std::min<int>(5, flat.size(0)); ++j)
            std::cout << flat[j].item<float>() << " ";
        std::cout << std::endl << "-----------------" << std::endl;

        ++count;
    }
}


torch::jit::script::Module loadTorchScriptModel(const std::string& model_path) {
    torch::jit::script::Module model;
    
    try {
        model = torch::jit::load(model_path);
        std::cout << "Model loaded successfully from: " << model_path << std::endl;
        printModelInfo(model);
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading model from: " << model_path << std::endl;
        throw;
    }
    return model;
}
