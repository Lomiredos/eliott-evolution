#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "IComponentArray.hpp"


namespace ee::ecs {

    using EntityID = uint32_t;
    template<typename T>
    class ComponentArray : public IComponentArray {

        private:
        
        std::vector<T> m_packedData;                     
        std::unordered_map<EntityID, size_t> m_index; 
        std::vector<EntityID> m_link;  

        public:
        void addComponent(EntityID _id, T _type){
            m_packedData.push_back(_type);
            m_index[_id] = m_packedData.size() - 1;
            m_link.push_back(_id);
        }

        void removeComponent(EntityID _id) override {

            if (!hasComponent(_id))
                return;

            //suppr de packedData
            size_t index = m_index[_id];
            std::swap(m_packedData[index], m_packedData[m_packedData.size()-1]);
            //mettre a jour les data
            EntityID endId = m_link[m_link.size()-1];
            m_index[endId] = index; 

            m_packedData.resize(m_packedData.size()-1);

            //supp de index
            m_index.erase(_id);
            
            //suppr de link
            std::swap(m_link[index], m_link[m_link.size()-1]);
            m_link.resize(m_link.size()-1);


        }

        T& getComponent(EntityID _id){
            size_t index = m_index[_id];
            return m_packedData[index];
        }
        
        bool hasComponent(EntityID _id) override {
            return m_index.find(_id) != m_index.end();
        }

        size_t getSize() override {
            return m_packedData.size();
        }
    };






}//namespace ee::ecs