#pragma once
#include <cstdint>
#include <queue>
#include <unordered_set>


namespace ee::ecs {

    using EntityID = uint32_t;

    
    class EntityManager {
        public:
        EntityID createEntity();
        void DestroyEntity(EntityID _id);
        bool isAlive(EntityID _id);
        size_t getEntityCount() const;
    private:
    std::queue<EntityID> m_availableIDs;
    std::unordered_set<EntityID> m_livingEntities;
    uint32_t m_nextID = 0;
    };



} // namespace ee::ecs