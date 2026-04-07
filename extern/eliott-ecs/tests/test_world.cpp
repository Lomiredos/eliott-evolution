#include <doctest/doctest.h>
#include "ecs/world.hpp"

using namespace ee::ecs;


TEST_CASE("Vérification si le createEntity renvoie un ID valide"){
    World W = World();
    CHECK(W.createEntity() == 0);
}


TEST_CASE("Vérification si le addComp / getComp fonctionne"){
    World W = World();
    W.createEntity();

    struct Position{
        float x;
        float y;
    };
    W.addComponent<Position>(0, {75, 75});
    CHECK(W.getComponent<Position>(0).x == 75);
}



TEST_CASE("Vérification si le destroyEntity le retire bien du system"){
    World W = World();
    W.createEntity();
    
    class PositionSystem : public System{};

    auto system = W.registerSystem<PositionSystem>();
    struct Position{
        float x;
        float y;
    };
    W.addComponent<Position>(0, {75, 75});

    CHECK(system->m_entities.contains(0));

    W.destroyEntity(0);

    CHECK(system->m_entities.contains(0) == false);

}