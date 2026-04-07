#pragma once
#include<unordered_map>
#include <memory>
#include <typeindex>
#include "System.hpp"




namespace ee::ecs {




    class SystemManager{

    private:
        std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;
        std::unordered_map<std::type_index, Signature> m_signatures;

    public:

        template<typename T>
        std::shared_ptr<T> registerSystem(){
            auto key = std::type_index(typeid(T));
            if (m_systems[key] == nullptr){
                m_systems[key] = std::make_shared<T>();
            }
            return std::static_pointer_cast<T>(m_systems[key]);
        }

        template<typename T>
        void setSignature(Signature _sign){
            auto key = std::type_index(typeid(T));
            if (m_systems[key] == nullptr)
                return;
            m_signatures[key] = _sign;
        }

        void onEntityDestroyed(EntityID _id){
            
            for (auto& [key, value] : m_systems) {
                value->m_entities.erase(_id);
            }
        }

        void onEntitySignatureChanged(EntityID _id, Signature _newSign){
            for (auto& [key, value] : m_systems) {
                if ((_newSign & m_signatures[key]) != m_signatures[key])
                    value->m_entities.erase(_id);
                else
                    value->m_entities.insert(_id);

            }
        }



    };
}