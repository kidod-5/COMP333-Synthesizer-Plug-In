/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class MLPlugInAudioProcessorEditor : public juce::AudioProcessorEditor {
  public:
    MLPlugInAudioProcessorEditor(MLPlugInAudioProcessor &);
    ~MLPlugInAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MLPlugInAudioProcessor &audioProcessor;

    juce::ComboBox noiseTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseTypeAttachment;
    juce::Slider noiseSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        noiseSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MLPlugInAudioProcessorEditor)
};
