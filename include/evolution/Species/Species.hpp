#pragma once

#include "evolution/Components/NeuralNetwork.hpp"
#include <string>
#include <vector>

struct Species {
    std::string name;
    float maxSpeed        = 150.f;
    float rotationSpeed   = 90.f;
    float maxHealth       = 100.f;
    float maxHunger       = 100.f;
    std::string foodType  = "generic";
    std::vector<int> topology = {5, 8, 2};

    int generation = 0;
    std::vector<float> fitnessMax;
    std::vector<float> fitnessMin;

    NeuralNetwork bestBrain;
    bool isEvolving = false;
};
