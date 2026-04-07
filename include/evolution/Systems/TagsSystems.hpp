#pragma once

#include "ecs/System.hpp"


class FoodSystem : public ee::ecs::System {
public:
    void update(ee::ecs::World&, float) override {} 

};
class WallSystem : public ee::ecs::System {
public:
    void update(ee::ecs::World&, float) override {} 

};