#include "../include/ecs/EntityManager.hpp"

ee::ecs::EntityID ee::ecs::EntityManager::createEntity()
{
    EntityID id;
    if (!m_availableIDs.empty()){
        id = m_availableIDs.front();
        m_availableIDs.pop();
    }
    else
    id = m_nextID++;
    
    m_livingEntities.insert(id);
    return id;
}

void ee::ecs::EntityManager::DestroyEntity(EntityID _id)
{
    if (!isAlive(_id))
    return;
    m_availableIDs.push(_id);
    m_livingEntities.erase(_id);
}

bool ee::ecs::EntityManager::isAlive(EntityID _id)
{
    return m_livingEntities.contains(_id);
}

size_t ee::ecs::EntityManager::getEntityCount() const
{
    return m_livingEntities.size();
}
