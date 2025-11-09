#pragma once

#include <JuceHeader.h>
#include "NoiseGenerator.h"
#include "WavetableOscillator.h"
#include "WavetableFactory.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent {
  public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock(
        const juce::AudioSourceChannelInfo &bufferToFill) override;

    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics &g) override;

    void resized() override;

  private:
    //==============================================================================
    // Your private member variables go here...
    juce::Random random;
    juce::Slider sineFreqSlider, sineLevelSlider, whiteNoiseLevelSlider, pinkNoiseLevelSlider;
    juce::Label sineFreqLabel, sineLevelLabel, whiteNoiseLevelLabel, pinkNoiseLevelLabel;
    
    juce::AudioSampleBuffer sineTable;
    const unsigned int tableSize = 1 << 7;
    
    std::unique_ptr<WavetableOscillator> oscillator;
    double currentSampleRate = 0.0;
    double currentFrequency = 440.0;
    double targetFrequency  = 440.0;
    float outputLevel = 0.15f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
