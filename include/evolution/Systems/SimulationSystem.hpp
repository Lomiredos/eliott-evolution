#pragma once

#include "evolution/Components/NeuralNetwork.hpp"

#include "evolution/Systems/TagsSystems.hpp"

#include "ecs/System.hpp"
#include "ecs/World.hpp"

#include <iostream>
#include <random>


#include <algorithm>

class SimulationSystem : public ee::ecs::System {

private:

    std::shared_ptr<FoodSystem> m_foodSystem;
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;


    float m_generationTime = 30.f;  
    float m_timer = 0.f;            
    int m_generation = 0;  

    std::vector<float> m_fitnessMax;
    std::vector<float> m_fitnessMin;
    NeuralNetwork m_bestBrain;

public:
    SimulationSystem(): m_rng(std::random_device{}()), m_dist(-1.0f, 1.0f) {}


    void init(std::shared_ptr<FoodSystem> _food)
    {
        m_foodSystem = _food;
    }

    void update(ee::ecs::World& _world, float _dt) override {

        m_timer += _dt;
        if (m_timer > m_generationTime){
            m_generation++;
            evolve(_world);
            m_timer = 0.f;
            return;
        }


        for (ee::ecs::EntityID id: m_entities){
            sf::Vector2f creaturePos = _world.getComponent<Transform>(id).position;
            NeuralNetwork& neural = _world.getComponent<NeuralNetwork>(id);
        
            for (ee::ecs::EntityID foodId : m_foodSystem->m_entities){
                sf::Vector2f foodPos = _world.getComponent<Transform>(foodId).position;
        
                if (getDistance(creaturePos, foodPos) > 15) continue;
            

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



int getGeneration() const { return m_generation; }
float getTimer() const { return m_timer; }
float getGenerationTime() const { return m_generationTime; }
std::vector<float> getFitnessMax() const { return m_fitnessMax; }
std::vector<float> getFitnessMin() const { return m_fitnessMin; }
const NeuralNetwork& getBestBrain() const { return m_bestBrain; }


private:
    float getDistance(const sf::Vector2f& a, const sf::Vector2f& b) const {
        return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
    }

    void evolve(ee::ecs::World& _world){
    std::uniform_real_distribution<float> distX(50.f, 1230.f);
    std::uniform_real_distribution<float> distY(50.f, 670.f);
    std::uniform_real_distribution<float> distDir(0.f, 360.f);



    std::vector<ee::ecs::EntityID> creatureID;
    for (ee::ecs::EntityID id : m_entities) creatureID.push_back(id);

    std::sort(creatureID.begin(), creatureID.end(), [&_world](const auto& a, const auto& b)
    {
        return _world.getComponent<NeuralNetwork>(a).fitness > _world.getComponent<NeuralNetwork>(b).fitness;  
    });

    m_fitnessMax.push_back(_world.getComponent<NeuralNetwork>(creatureID[0]).fitness);
    m_fitnessMin.push_back(_world.getComponent<NeuralNetwork>(creatureID[creatureID.size() -1 ]).fitness);
    m_bestBrain = _world.getComponent<NeuralNetwork>(creatureID[0]);


    int nbParents = std::max(1, (int)(creatureID.size() * 0.1f));
    creatureID.erase(creatureID.begin() + nbParents, creatureID.end());

    std::vector<NeuralNetwork> parentsBrains;
    for (ee::ecs::EntityID id : creatureID) parentsBrains.push_back(_world.getComponent<NeuralNetwork>(id));
    
    creatureID.clear();

    for (ee::ecs::EntityID id : m_entities){
        _world.markForDestruction(id);
    }

    std::uniform_int_distribution<int> distP(0, nbParents - 1);

    for (int i = 0; i < nbParents * 10; i++){

        NeuralNetwork child = mutate(parentsBrains[distP(m_rng)]);

        ee::ecs::EntityID creature = _world.createEntity();
        _world.addComponent(creature, Transform{ sf::Vector2f(distX(m_rng), distY(m_rng)) });
        _world.addComponent(creature, Motion{ distDir(m_rng), 90.f, 0.f, 150.f });
        _world.addComponent(creature, Vitals{ 100.f, 100.f, 0.f });
        std::vector<Ray> rays;
        for (int i = -2; i <= 2; i++){
            rays.push_back(Ray{100.f, (float)(i * 30), 100.f, DetectedType::Nothing});
        }
        _world.addComponent(creature, Vision{rays});
        _world.addComponent(creature, child);
    }

    for (int i = 0; i < 10; i++){
        ee::ecs::EntityID food = _world.createEntity();
        _world.addComponent(food, Transform{ sf::Vector2f(distX(m_rng), distY(m_rng)) });
        _world.addComponent(food, Food{});
    }


    }

    NeuralNetwork mutate(const NeuralNetwork& _parent)
    {
        NeuralNetwork child = _parent;  
        
        std::uniform_real_distribution<float> mutation(-0.1f, 0.1f);
        
        for (int i = 0; i< child.weights.size(); i++){
            for (int j = 0; j < child.weights[i].size(); j++){
                child.weights[i][j] += mutation(m_rng);
            }
        }
        for (int i = 0; i< child.biases.size(); i++){
            for (int j = 0; j < child.biases[i].size(); j++){
                child.biases[i][j] += mutation(m_rng);
            }
        }
        child.fitness = 0.f; 
        return child;
    }

};