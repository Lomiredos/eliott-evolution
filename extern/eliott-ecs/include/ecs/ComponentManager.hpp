#pragma once
#include <cstdint>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "ComponentArray.hpp"

namespace ee::ecs 
{
    
    using EntityID = uint32_t;


    class ComponentManager{

        private:
        std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_arrays;


        public:
        template<typename T>
        void addComponent(EntityID _id, T _comp){
            auto key = std::type_index(typeid(T));
            if (m_arrays[key] == nullptr)
                m_arrays[key] = std::make_shared<ComponentArray<T>>();
        
            std::static_pointer_cast<ComponentArray<T>>(m_arrays[key])->addComponent(_id, _comp);
        }
        template<typename T>
        void removeComponent(EntityID _id){
            auto key = std::type_index(typeid(T));
            if (m_arrays[key] == nullptr)
                return;
            m_arrays[key]->removeComponent(_id);
        }
        template<typename T>
        T& getComponent(EntityID _id){
            auto key = std::type_index(typeid(T));

            return std::static_pointer_cast<ComponentArray<T>>(m_arrays[key])->getComponent(_id);
        }

        void onEntityDestroyed(EntityID _id){

            for (auto& [key, value] : m_arrays) {
               value->removeComponent(_id);
            }
        }
        template<typename T>
        bool hasComponent(EntityID _id){
            auto key = std::type_index(typeid(T));
            if (m_arrays[key] == nullptr)
                return false;
            return m_arrays[key]->hasComponent(_id);
            
        }

        int getSize() {
            return m_arrays.size();
        } 

    };

} // namespace ee::ecs 

        







