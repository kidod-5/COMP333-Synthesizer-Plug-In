/*
  ==============================================================================

    WavetableOscillator.h
    Created: 2 Nov 2025 3:55:56pm
    Author:  Sam

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WavetableOscillator {
  public:
    WavetableOscillator(const juce::AudioSampleBuffer &tableToUse)
        : wavetable(tableToUse), tableSize(wavetable.getNumSamples() - 1) {
        jassert(wavetable.getNumChannels() == 1);
    }

    void setFrequency(double frequency, double sampleRate) {
        auto tableSizeOverSampleRate = (float)tableSize / (float)sampleRate;
        tableDelta = (float)frequency * tableSizeOverSampleRate;
    }

    forcedinline float getNextSample() noexcept {
        auto index0 = (unsigned int)currentIndex;
        auto index1 = index0 + 1;
        auto frac = currentIndex - (float)index0;

        auto *table = wavetable.getReadPointer(0);

        auto value = table[index0] + frac * (table[index1] - table[index0]);

        currentIndex += tableDelta;
        if (currentIndex >= (float)tableSize)
            currentIndex -= (float)tableSize;

        return value;
    }

  private:
    const juce::AudioSampleBuffer &wavetable;
    const int tableSize;
    float currentIndex = 0.0f;
    float tableDelta = 0.0f;
};
