#pragma once
#include <cmath>
#include "evolution/Components/Vision.hpp"
#include "evolution/Components/Transform.hpp"
#include "evolution/Components/Motion.hpp"
#include "ecs/World.hpp"

#include "evolution/Systems/TagsSystems.hpp"


class VisionSystem : public ee::ecs::System {
private:
    std::shared_ptr<FoodSystem> m_foodSystem;
    std::shared_ptr<WallSystem> m_wallSystem;

    public:

    void init(std::shared_ptr<FoodSystem> _food, std::shared_ptr<WallSystem> _wall)
    {
        m_foodSystem = _food;
        m_wallSystem = _wall;
    }



    void update(ee::ecs::World& _world, float _dt)  override {

        for (ee::ecs::EntityID id : m_entities){
            Transform& transform = _world.getComponent<Transform>(id);
            Motion& motion = _world.getComponent<Motion>(id);
            Vision& vision = _world.getComponent<Vision>(id);

            for (Ray& ray : vision.rays){
                float D = (motion.direction + ray.rotation) * (M_PI / 180.0f);
                
                ray.detected = DetectedType::Nothing;
                ray.distance = ray.maxRange;

                for (ee::ecs::EntityID foodID : m_foodSystem->m_entities){
                    sf::Vector2f foodPos = _world.getComponent<Transform>(foodID).position;
                    
                    float fx = foodPos.x - transform.position.x; 
                    float fy = foodPos.y - transform.position.y;
                    
                    float dot = fx * cos(D) + fy * sin(D);
                    float perp = std::abs(fx * sin(D) - fy* cos(D));

                    if (dot < 0 || dot > ray.distance || perp > 15.0f) continue;
                    
                    ray.detected = DetectedType::Food;
                    ray.distance = dot;
                }


                for (ee::ecs::EntityID wallID : m_wallSystem->m_entities){
                    sf::Vector2f wallPos = _world.getComponent<Transform>(wallID).position;
                    
                    float fx = wallPos.x - transform.position.x; 
                    float fy = wallPos.y - transform.position.y;
                    
                    float dot = fx * cos(D) + fy * sin(D);
                    float perp = std::abs(fx * sin(D) - fy* cos(D));

                    if (dot < 0 || dot > ray.distance || perp > 15.0f) continue;
                    
                    ray.detected = DetectedType::Wall;
                    ray.distance = dot;
                }

            }
        }

    }
};