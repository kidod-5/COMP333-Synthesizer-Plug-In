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

                writePtr[i] = pink * 0.05f * amplitude;  // scaled to avoid clipping
            }
        }
    }
};                              
                                   
                                   

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <string.h>
//#include <time.h>
//
//// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
//// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
//
//typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;
//
//uint32_t pcg32_random_r(pcg32_random_t* rng)
//{
//    uint64_t oldstate = rng->state;
//    // Advance internal state
//    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
//    // Calculate output function (XSH RR), uses old state for max ILP
//    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
//    uint32_t rot = oldstate >> 59u;
//    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
//}
//
//int main(int argc, unsigned char *argv[])
//// Voss-McCartney algorithm
//{
// FILE            *outfile;
// unsigned long   filesize=0, count;
// uint16_t        num_zeroes;
// int16_t         i,sample16;
// uint32_t        rand_ary[16];
// long long       sum,total;
// pcg32_random_t  rng;
// struct timespec tv;
// 
// if (argc <2) { printf("No output file specified.\n"); return -1; }
// if (argc <3) { printf("No file size specified.\n"); return -1; }
// if((outfile =fopen(argv[1],"wb")) == ((FILE *)NULL)) {printf("Failed to open %s: %s\n",argv[1],strerror(errno)); goto bad;}
// filesize = strtol(argv[2],NULL,0);
//printf("filename=%s, filesize=%ld\n",argv[1],filesize);
//
//  // Initialize random number generator.
//  clock_gettime(CLOCK_MONOTONIC, &tv);
//  rng.state = tv.tv_nsec; // Arbitrary starting point in random sequence.
//  rng.inc   = 0x12345;    // Arbitrary choice of which sequence to generate.
//  for(i=0,sum=0ll; i<16; i++) sum+=(rand_ary[i]=pcg32_random_r(&rng));
//
//  for (count=0; count<filesize; count++)
//  {
//   num_zeroes = __builtin_ctz(((unsigned int)count)|0x4000); // Force num_zeroes to always be in range 0-14.
//   sum -= rand_ary[num_zeroes];
//   rand_ary[num_zeroes] = pcg32_random_r(&rng);
//   sum += rand_ary[num_zeroes];
//   total = sum +          pcg32_random_r(&rng);
//   total = (sum >> 20) - 32768;
//   sample16 = (int16_t) total;
//   fwrite (&sample16, sizeof(int16_t), 1, outfile);
//  }
//bad:
// fclose(outfile);
//}
