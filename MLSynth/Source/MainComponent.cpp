#include "MainComponent.h"
#include "NoiseGenerator.h"
#include "WavetableFactory.h"
#include "WavetableOscillator.h"

//==============================================================================
MainComponent::MainComponent() {
    // Make sure you set the size of the component after
    // you add any child components.

    // Set up Sine Wavetable
    sineFreqSlider.setRange(50.0, 5000.0);
    sineFreqSlider.setSkewFactorFromMidPoint(500.0);
    sineFreqSlider.setValue(440.0);
    sineFreqSlider.onValueChange = [this] {
        targetFrequency = sineFreqSlider.getValue();
    };
    sineFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    sineFreqLabel.setText("Sine Freq", juce::dontSendNotification);
    
    sineLevelSlider.setRange(0.0, 0.5);
    sineLevelSlider.setValue(outputLevel);
    sineLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    sineLevelLabel.setText("Sine Level", juce::dontSendNotification);
    
    sineTable = WavetableFactory::createSineTable((int)tableSize);

    // Set up noise sliders
    whiteNoiseLevelSlider.setRange(0.0, 0.25);
    whiteNoiseLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                          100, 20);
    whiteNoiseLevelLabel.setText("White Noise Level",
                                 juce::dontSendNotification);

    pinkNoiseLevelSlider.setRange(0.0, 0.25);
    pinkNoiseLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100,
                                         20);
    pinkNoiseLevelLabel.setText("Pink Noise Level", juce::dontSendNotification);

    // Show sliders with labels
    addAndMakeVisible(sineFreqSlider);
    addAndMakeVisible(sineFreqLabel);
    addAndMakeVisible(sineLevelSlider);
    addAndMakeVisible(sineLevelLabel);

    addAndMakeVisible(whiteNoiseLevelSlider);
    addAndMakeVisible(whiteNoiseLevelLabel);

    addAndMakeVisible(pinkNoiseLevelSlider);
    addAndMakeVisible(pinkNoiseLevelLabel);

    // ---
    setSize(800, 600);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() {
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected,
                                  double sampleRate) {
    juce::String message;
    message << "Preparing to play audio...\n";
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
    message << " sampleRate = " << sampleRate;
    juce::Logger::getCurrentLogger()->writeToLog(message);

    currentSampleRate = sampleRate;
    oscillator = std::make_unique<WavetableOscillator>(sineTable);
    oscillator->setFrequency(currentFrequency, currentSampleRate);
}

void MainComponent::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
    auto *buffer = bufferToFill.buffer;
    auto start = bufferToFill.startSample;
    auto numSamples = bufferToFill.numSamples;

    // 1) Clear buffer each block
    buffer->clear(start, numSamples);

    // 2) Handle frequency smoothing
    if (!juce::approximatelyEqual(targetFrequency, currentFrequency)) {
        auto step = (targetFrequency - currentFrequency) / numSamples;
        for (int i = 0; i < numSamples; ++i) {
            currentFrequency += step;
            oscillator->setFrequency(currentFrequency, currentSampleRate);

            float sample = oscillator->getNextSample() * (float)sineLevelSlider.getValue();

            for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
                buffer->addSample(ch, start + i, sample);
        }
    } else {
        for (int i = 0; i < numSamples; ++i) {
            float sample = oscillator->getNextSample() * outputLevel;

            for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
                buffer->addSample(ch, start + i, sample);
        }
    }

    // 3) Add Noise
    float whiteNoiseLevel = (float)whiteNoiseLevelSlider.getValue();
    float pinkNoiseLevel = (float)pinkNoiseLevelSlider.getValue();

    juce::AudioBuffer<float> temp(buffer->getNumChannels(), numSamples);

    // White noise
    temp.clear();
    NoiseGenerator::whiteNoiseGenerator(temp, 0, numSamples, whiteNoiseLevel);
    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        buffer->addFrom(ch, start, temp, ch, 0, numSamples);

    // Pink noise
    temp.clear();
    NoiseGenerator::pinkNoiseGenerator(temp, 0, numSamples, pinkNoiseLevel);
    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        buffer->addFrom(ch, start, temp, ch, 0, numSamples);
}

void MainComponent::releaseResources() {
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
}

//==============================================================================
void MainComponent::paint(juce::Graphics &g) {
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized() {
    int labelWidth = 100;
    int sliderHeight = 20;
    int y = 10;

    sineFreqLabel.setBounds(10, 7, labelWidth, sliderHeight);
    sineFreqSlider.setBounds(120, y, getWidth() - 130, sliderHeight);
    
    y += 40;

    sineLevelLabel.setBounds(10, y, labelWidth, sliderHeight);
    sineLevelSlider.setBounds(120, y, getWidth() - 130, sliderHeight);
    
    y += 40;

    whiteNoiseLevelLabel.setBounds(10, y, labelWidth, sliderHeight);
    whiteNoiseLevelSlider.setBounds(120, y, getWidth() - 130, sliderHeight);

    y += 40;

    pinkNoiseLevelLabel.setBounds(10, y, labelWidth, sliderHeight);
    pinkNoiseLevelSlider.setBounds(120, y, getWidth() - 130, sliderHeight);
}
