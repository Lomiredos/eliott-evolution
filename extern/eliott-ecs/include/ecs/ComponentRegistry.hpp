#pragma once
#include <bitset>
#include <cstdint>

namespace ee::ecs {

    using Signature = std::bitset<64>;
    using EntityID = uint32_t;

    

    inline size_t s_counter = 0;

    template<typename T>
    size_t getComponentID() {
        static size_t id = s_counter++;
        return id;
    }
}