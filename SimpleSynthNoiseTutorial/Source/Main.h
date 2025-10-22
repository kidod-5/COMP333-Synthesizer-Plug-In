/*
 ==============================================================================

 This file is part of the JUCE tutorials.
 Copyright (c) 2020 - Raw Material Software Limited

 The code included in this file is provided under the terms of the ISC license
 http://www.isc.org/downloads/software-support-policy/isc-license. Permission
 To use, copy, modify, and/or distribute this software for any purpose with or
 without fee is hereby granted provided that the above copyright notice and
 this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
 WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
 PURPOSE, ARE DISCLAIMED.

 ==============================================================================
 */

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SimpleSynthNoiseTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Simple noise synthesiser.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
 juce_audio_processors, juce_audio_utils, juce_core,
 juce_data_structures, juce_events, juce_graphics,
 juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

 *******************************************************************************/

#pragma once
#include <JuceHeader.h>
#include "NoiseGenerator.h"

//==============================================================================
class MainContentComponent : public juce::AudioAppComponent {
  public:
    MainContentComponent() {
        levelSlider.setRange(0.0, 0.25);
        levelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
        levelLabel.setText("Noise Level", juce::dontSendNotification);

        setSize(800, 600);
        setAudioChannels(0, 2); // no inputs, two outputs
    }

    ~MainContentComponent() override {
        shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected,
                       double sampleRate) override {
        juce::String message;
        message << "Preparing to play audio...\n";
        message << " samplesPerBlockExpected = " << samplesPerBlockExpected
                << "\n";
        message << " sampleRate = " << sampleRate;
        juce::Logger::getCurrentLogger()->writeToLog(message);
    }

    void releaseResources() override {
        juce::Logger::getCurrentLogger()->writeToLog(
            "Releasing audio resources");
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        float level = (float)levelSlider.getValue();
        NoiseGenerator::fillWithNoise(*bufferToFill.buffer,
                                      bufferToFill.startSample,
                                      bufferToFill.numSamples,
                                      level);
    }

  private:
    juce::Slider levelSlider;
    juce::Label levelLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
