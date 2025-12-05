/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

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
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        noiseTypeAttachment;

    juce::Slider noiseSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        noiseSliderAttachment;

    juce::ComboBox modelBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        modelAttachment;

    juce::Slider wetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        wetSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MLPlugInAudioProcessorEditor)
};
