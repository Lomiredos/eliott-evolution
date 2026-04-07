#pragma once
#include <vector>


struct NeuralNetwork {

    std::vector<std::vector<float>> weights;
    std::vector<std::vector<float>> biases;
    
    float fitness;
};