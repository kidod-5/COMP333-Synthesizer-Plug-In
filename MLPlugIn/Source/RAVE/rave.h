/*
  ==============================================================================

    Load torchscript PyTorch pre-trained model and display some parameters

  ==============================================================================
*/

#pragma once
#include <torch/torch.h>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

void printModelInfo(const torch::jit::script::Module &model, int max_layers);

torch::jit::script::Module
loadTorchScriptModelAsync(const std::string &model_path);

torch::Tensor audioBufferToTensor(const juce::AudioBuffer<float> &buffer);

void tensorToAudioBuffer(const torch::Tensor &tensor,
                         juce::AudioBuffer<float> &buffer);
