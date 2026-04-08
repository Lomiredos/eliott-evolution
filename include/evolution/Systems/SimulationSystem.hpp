#pragma once

#include "evolution/Components/NeuralNetwork.hpp"

#include "evolution/Systems/TagsSystems.hpp"

#include "ecs/System.hpp"
#include "ecs/World.hpp"

#include <iostream>
#include <random>


class SimulationSystem : public ee::ecs::System {

private:

    std::shared_ptr<FoodSystem> m_foodSystem;
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;


public:
    SimulationSystem(): m_rng(std::random_device{}()), m_dist(-1.0f, 1.0f) {}


    void init(std::shared_ptr<FoodSystem> _food)
    {
        m_foodSystem = _food;
    }

    void update(ee::ecs::World& _world, float _dt) override {
        for (ee::ecs::EntityID id: m_entities){
            sf::Vector2f creaturePos = _world.getComponent<Transform>(id).position;
            NeuralNetwork& neural = _world.getComponent<NeuralNetwork>(id);
        
            for (ee::ecs::EntityID foodId : m_foodSystem->m_entities){
                sf::Vector2f foodPos = _world.getComponent<Transform>(foodId).position;
        
                if (getDistance(creaturePos, foodPos) > 15) continue;
                
std::cout << "Food mangee ! Distance: " 
          << getDistance(creaturePos, foodPos) << std::endl;


                neural.fitness++;
                _world.markForDestruction(foodId);

            }
        }
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

    private:
    float getDistance(const sf::Vector2f& a, const sf::Vector2f& b) const {
        return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
    }

};