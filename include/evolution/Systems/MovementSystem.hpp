#pragma once

#include <cmath>

#include "ecs/System.hpp"
#include "ecs/World.hpp"
#include "evolution/Components/Transform.hpp"
#include "evolution/Components/Motion.hpp"


class MovementSystem : public  ee::ecs::System {

    public:
    
    void update(ee::ecs::World& _world, float _dt) override{


        for (ee::ecs::EntityID id : m_entities){
            Transform& transform = _world.getComponent<Transform>(id);
            Motion& motion = _world.getComponent<Motion>(id);

            float dx = cos(motion.direction);
            float dy = sin(motion.direction);

            sf::Vector2f dir = sf::Vector2f(dx, dy);
            sf::Vector2f movement = dir * motion.speed * _dt;
            transform.position += movement;
        }
    }
};