#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include <unordered_map>


namespace ee::ecs {
    class World{

        private:
        
        ComponentManager m_compManager;
        EntityManager m_entityManager;
        SystemManager m_sysManager;
        std::unordered_map<EntityID, Signature> m_signatures;
    
        public:
        EntityID createEntity() {
            return m_entityManager.createEntity();

        }
        void destroyEntity(EntityID _id) {
            
            m_sysManager.onEntityDestroyed(_id);
            m_compManager.onEntityDestroyed(_id);
            m_entityManager.DestroyEntity(_id);
            m_signatures.erase(_id);
        
        }
        
        template<typename T>
        void addComponent(EntityID _id, T _comp) {
            m_compManager.addComponent(_id, _comp);
            m_signatures[_id].set(getComponentID<T>());
            m_sysManager.onEntitySignatureChanged(_id, m_signatures[_id]);

        }

        template<typename T>
        void removeComponent(EntityID _id) {
            m_compManager.removeComponent<T>(_id);
            m_signatures[_id].reset(getComponentID<T>());
            m_sysManager.onEntitySignatureChanged(_id, m_signatures[_id]);
        }

        template<typename T>
        T& getComponent(EntityID _id) {

            return m_compManager.getComponent<T>(_id);
        }

        template<typename T>
        bool hasComponent(EntityID _id) {
            return m_compManager.hasComponent<T>(_id);
        }

        template<typename T>
        std::shared_ptr<T> registerSystem() {

            return m_sysManager.registerSystem<T>();
        }

        template<typename T>
        void setSystemSignature(Signature _newSign) {

            m_sysManager.setSignature<T>(_newSign);
        }
    
    };

}