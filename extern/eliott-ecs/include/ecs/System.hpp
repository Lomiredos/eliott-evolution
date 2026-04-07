#pragma once
#include <set>
#include "ComponentRegistry.hpp"

namespace ee::ecs {


    class World;

    
    class System
    {
    public:
        std::set<EntityID> m_entities;
        virtual ~System() = default;

        virtual void update(World& _world, float _dt) = 0;

    };

}