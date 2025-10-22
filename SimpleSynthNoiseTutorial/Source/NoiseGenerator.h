//
//  NoiseGenerator.h
//  SimpleSynthNoiseTutorial - App
//
//  Created by Sam on 10/21/25.
//  Copyright © 2025 JUCE. All rights reserved.
//

#pragma once
#include <JuceHeader.h>

class NoiseGenerator {
public:
    NoiseGenerator() = default;

    // Generates white noise into a buffer for a given channel.
    static void fillWithNoise(juce::AudioBuffer<float>& buffer,
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
};
