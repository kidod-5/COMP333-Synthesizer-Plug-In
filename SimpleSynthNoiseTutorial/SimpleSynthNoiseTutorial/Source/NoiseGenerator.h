/*
  ==============================================================================

    NoiseGenerator.h
    Created: 21 Oct 2025 10:25:39am
    Author:  Sam

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class NoiseGenerator {
  public:
    NoiseGenerator() = default;

    static void whiteNoiseGenerator(juce::AudioBuffer<float>& buffer,
                                    int startSample,
                                    int numSamples,
                                    float amplitude) {
        juce::Random random;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            float* writePtr = buffer.getWritePointer(channel, startSample);
            for (int sample = 0; sample < numSamples; ++sample)
                writePtr[sample] = (random.nextFloat() * 2.0f - 1.0f) * amplitude;
        }
    }
    
    static void pinkNoiseGenerator(juce::AudioBuffer<float>& buffer,
                                  int startSample,
                                  int numSamples,
                                  float amplitude) {
        juce::Random random;

        // Filter state (should persist if used continuously — here it's per call)
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            float* writePtr = buffer.getWritePointer(channel, startSample);

            for (int i = 0; i < numSamples; ++i) {
                float white = random.nextFloat() * 2.0f - 1.0f;

                // Paul Kellet filter
                b0 = 0.99765f * b0 + white * 0.0990460f;
                b1 = 0.96300f * b1 + white * 0.2965164f;
                b2 = 0.57000f * b2 + white * 1.0526913f;
                b3 = 0.86650f * b3 + white * 0.3104856f;
                b4 = 0.55000f * b4 + white * 0.5329522f;
                b5 = -0.7616f * b5 - white * 0.0168980f;
                float pink = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.1848f;

                writePtr[i] = pink * 0.5f * amplitude;  // scaled to avoid clipping
            }
        }
    }
};
