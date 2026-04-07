#include <doctest/doctest.h>
#include "ecs/ComponentArray.hpp"

using namespace ee::ecs;


TEST_CASE("Vérification ajout component"){

    struct Position{
        float x;
        float y;
    };

    ComponentArray<Position> array;
    array.addComponent(0, {25, 75});
    CHECK(array.getSize() == 1);
}

TEST_CASE("Vérification supprésion components"){
    struct Position{
        float x;
        float y;
    };

    ComponentArray<Position> array;
    array.addComponent(0, {25, 75});
    array.removeComponent(0);
    CHECK(array.getSize() == 0);
}

TEST_CASE("Vérification hasComponent"){
    struct Position{
        float x;
        float y;
    };

    ComponentArray<Position> array;
    
    CHECK(array.hasComponent(1) == false);
}

TEST_CASE("Vérification getComponent"){
    struct Position{
        float x;
        float y;
    };

    ComponentArray<Position> array;
    array.addComponent(0, {25, 75});
    Position pos = array.getComponent(0);
    CHECK(pos.x == 25);
}

TEST_CASE("Vérification du swap-remove"){
    struct Position{
        float x;
        float y;
    };

    ComponentArray<Position> array;
    array.addComponent(0, {25, 75});
    array.addComponent(1, {75, 25});
    array.addComponent(2, {0, 0});
    array.removeComponent(1);
    CHECK(array.getComponent(2).x == 0);
}