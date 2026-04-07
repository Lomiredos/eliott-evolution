#pragma once

#include "evolution/Components/Vision.hpp"
#include "evolution/Components/NeuralNetwork.hpp"
#include "evolution/Components/Motion.hpp"
#include "ecs/World.hpp"
#include "ecs/System.hpp"

#include <vector>
#include <cmath>

class NeuralSystem : public ee::ecs::System {


public:
     void update(ee::ecs::World& _world, float _dt) override {

        for (ee::ecs::EntityID id : m_entities){
            Motion& motion = _world.getComponent<Motion>(id);
            Vision& vision = _world.getComponent<Vision>(id);
            NeuralNetwork& neuralNetwork = _world.getComponent<NeuralNetwork>(id);

            std::vector<float> entry;
            for (const Ray& ray : vision.rays) entry.push_back(rayToFloat(ray));


            std::vector<float> move = forward(neuralNetwork, entry);
            

            motion.direction += move[0] * _dt * motion.rotationSpeed;
            motion.speed = move[1] * motion.maxSpeed;
        }
     }

private:
     float rayToFloat(const Ray& _ray) const {
        float typeValue;
        switch (_ray.detected){
            case(DetectedType::Nothing): typeValue = 0.f; break;
            case(DetectedType::Food): typeValue = 1.f; break;
            case(DetectedType::Wall): typeValue = -1.f; break;
            default: typeValue = 0.f; break;
        };

        float proximity = 1.f - (_ray.distance / _ray.maxRange);
        return typeValue * proximity;
     }

     std::vector<float> forward(const NeuralNetwork& _nn, const std::vector<float>& _entry) const {

        std::vector<float> current = _entry;

        for (size_t i = 0; i < _nn.weights.size(); i++){
            std::vector<float> outputs;

            for (size_t j = 0; j < _nn.biases[i].size(); j++){
                float sum = _nn.biases[i][j];
                for (size_t k = 0; k < current.size(); k++) sum += current[k] * _nn.weights[i][j * current.size() + k];
                outputs.push_back(tanh(sum));
            }
            current = outputs;
        }
        return current;
     }


    };