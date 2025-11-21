/*
  ==============================================================================

    Test script for libtorch

  ==============================================================================
*/

#include "test_libtorch.h"
#include <iostream>
#include <torch/torch.h>

void testLibtorch() {
    torch::Tensor tensor = torch::ones({2, 3});
    torch::Tensor result = tensor * 2;

    std::cout << "Tensor:\n" << tensor << std::endl;
    std::cout << "Tensor * 2:\n" << result << std::endl;
}
