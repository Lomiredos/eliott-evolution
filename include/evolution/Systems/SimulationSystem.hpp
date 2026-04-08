#pragma once

#include "evolution/Components/NeuralNetwork.hpp"


#include "ecs/System.hpp"
#include "ecs/World.hpp"

#include <random>


class SimulationSystem : public ee::ecs::System {

private:
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;


public:
    SimulationSystem(): m_rng(std::random_device{}()), m_dist(-1.0f, 1.0f) {}

    void update(ee::ecs::World& _world, float _dt) override {

    }


    NeuralNetwork createRandom(const std::vector<int>& _topology) {
        NeuralNetwork nn;

        for (size_t i = 0; i < _topology.size() - 1; i++){
            
            int nbEntrees = _topology[i];
            int nbNeurones = _topology[i +1];

            std::vector<float> layerWeights;
            for (size_t j = 0; j < nbEntrees * nbNeurones; j++) layerWeights.push_back(m_dist(m_rng));
            nn.weights.push_back(layerWeights);
            
            std::vector<float> layerBiases;
            for (size_t j = 0; j < nbNeurones; j++) layerBiases.push_back(m_dist(m_rng));
            nn.biases.push_back(layerBiases);
        }
        nn.fitness = 0.f;
        return nn;
    }


};