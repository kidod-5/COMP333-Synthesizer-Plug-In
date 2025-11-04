/*
  ==============================================================================

    WavetableFactory.h
    Created: 2 Nov 2025 4:42:07pm
    Author:  Sam

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

 namespace WavetableFactory {
 inline juce::AudioBuffer<float> createSineTable (int tableSize) {
     juce::AudioBuffer<float> table (1, tableSize + 1);
     auto* samples = table.getWritePointer (0);

     auto angleDelta = juce::MathConstants<float>::twoPi / (float) tableSize;
     float angle = 0.0f;

     for (unsigned int i = 0; i < tableSize; ++i) {
         samples[i] = std::sin (angle);
         angle += angleDelta;
     }

     samples[tableSize] = samples[0]; // Wrap around

     return table;

 }
 }


