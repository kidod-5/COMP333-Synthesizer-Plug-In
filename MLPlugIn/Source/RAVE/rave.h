/*
  ==============================================================================

    Load torchscript PyTorch pre-trained model and display some parameters

  ==============================================================================
*/

#pragma once
#include <torch/torch.h>

void printModelInfo(const torch::jit::script::Module& model, int max_layers);

torch::jit::script::Module loadTorchScriptModel(const std::string& model_path);
