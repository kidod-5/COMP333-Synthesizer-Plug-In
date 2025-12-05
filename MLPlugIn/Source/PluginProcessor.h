/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <torch/torch.h>

#include "AudioGenerators/Noise.h"
#include "RAVE/RaveModelManager.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

//==============================================================================
/**
 */
class MLPlugInAudioProcessor : public juce::AudioProcessor {
  public:
    //==============================================================================
    MLPlugInAudioProcessor();
    ~MLPlugInAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioBuffer<float> inputBuffer; // buffer to accumulate 2048 samples
    int writePos = 0;
    constexpr static int blockSize = 2048;

    //==============================================================================
    //    float noiseAmplitude = 0.2f;
    std::atomic<float> *noiseAmplitudeParam = nullptr;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> *noiseTypeParam =
        nullptr; // 0 = white noise, 1 = pink noise
//    std::atomic<float> *modelChoiceParam = nullptr;
    std::atomic<float> *wetParam = nullptr;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;
    void scanModelDirectory();
    juce::StringArray getAvailableModels() const { return availableModels; }
    void loadSelectedModel(const juce::String& name);
    

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

  private:
    //==============================================================================
    torch::jit::script::Module model;
    std::atomic<bool> isModelLoaded{false};
    RaveModelManager modelManager;
    juce::StringArray availableModels;
    juce::String selectedModelName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MLPlugInAudioProcessor)
};
