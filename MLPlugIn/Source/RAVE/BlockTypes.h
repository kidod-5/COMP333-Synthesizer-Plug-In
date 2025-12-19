/*
  ==============================================================================

    Set up block to be sent to the FIFO

  ==============================================================================
*/

#pragma once
#include <array>
#include <cstdint>

constexpr int RAVE_BLOCK_SIZE =
    16384; // From the test_rave.py script (tried different numbers)

struct AudioBlock {
    uint64_t seq = 0;
    std::array<float, RAVE_BLOCK_SIZE> samplesL;
    std::array<float, RAVE_BLOCK_SIZE> samplesR;
};
