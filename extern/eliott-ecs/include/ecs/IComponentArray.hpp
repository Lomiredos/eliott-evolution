#pragma once
#include <cstdint>


namespace ee::ecs {


    using EntityID = uint32_t;


    class IComponentArray{
        public:
        virtual ~IComponentArray() = default;
        virtual void removeComponent(EntityID _id) = 0;
        virtual bool hasComponent(EntityID _id) = 0;
        virtual size_t getSize() = 0;
    };
}