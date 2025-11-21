/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

MLPlugInAudioProcessorEditor::MLPlugInAudioProcessorEditor(
    MLPlugInAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(400, 300);

    noiseTypeBox.addItem("White", 1);
    noiseTypeBox.addItem("Pink", 2);
//    addAndMakeVisible(noiseTypeBox);

    noiseTypeAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "noiseType", noiseTypeBox);

    noiseTypeBox.setSelectedId(
        static_cast<int>(audioProcessor.noiseTypeParam->load()) + 1,
        juce::dontSendNotification);

    noiseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noiseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);

//    addAndMakeVisible(noiseSlider);

    noiseSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "noiseAmplitude", noiseSlider);

    // Ensure slider reflects the parameter value on creation
    noiseSlider.setValue(*audioProcessor.noiseAmplitudeParam,
                         juce::dontSendNotification);

    wetSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(wetSlider);

    wetSliderAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "wet", wetSlider);

    // Ensure slider reflects the parameter value on creation
    wetSlider.setValue(*audioProcessor.wetParam, juce::dontSendNotification);
}

MLPlugInAudioProcessorEditor::~MLPlugInAudioProcessorEditor() {}

//==============================================================================
void MLPlugInAudioProcessorEditor::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
//    g.drawFittedText("ML PlugIn!", getLocalBounds(),
//                     juce::Justification::centred, 1);
}

void MLPlugInAudioProcessorEditor::resized() {
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int margin = 40;
    int controlHeight = 40;
    int spacing = 20;

    int y = spacing;
    noiseTypeBox.setBounds(margin, y, getWidth() - 2 * margin, controlHeight);
    y += controlHeight + spacing;

    noiseSlider.setBounds(margin, y, getWidth() - 2 * margin, controlHeight);
    y += controlHeight + spacing;

    wetSlider.setBounds(margin, y, getWidth() - 2 * margin, controlHeight);
}
