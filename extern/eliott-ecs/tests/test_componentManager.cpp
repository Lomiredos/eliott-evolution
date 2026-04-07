#include <doctest/doctest.h>
#include "ecs/ComponentManager.hpp"

using namespace ee::ecs;


TEST_CASE("Vérification de l'ajout de vecteur type"){
    
    struct Position{
        float x;
        float y;
    };

    struct Vector2D{
        float first;
        float second;
    };
    struct Motion{
        Vector2D velocity;
    };
    
    ComponentManager CM = ComponentManager();
    
    CM.addComponent<Position>(0, {75, 75});

    CM.addComponent<Motion>(0, {{1, 1}});

    CHECK(CM.getSize() == 2);
}


TEST_CASE("Vérification du removeComponent"){
    
    struct Position{
        float x;
        float y;
    };
    ComponentManager CM = ComponentManager();
    
    CM.addComponent<Position>(0, {75, 75});
    
    CM.removeComponent<Position>(0);
    
    CHECK(CM.hasComponent<Position>(0) == false);
}

TEST_CASE("Vérification du onentityDestroyed"){
    
    struct Position{
        float x;
        float y;
    };
   ComponentManager CM = ComponentManager();
    
    CM.addComponent<Position>(0, {75, 75});

    CM.onEntityDestroyed(0);

    CHECK(CM.hasComponent<Position>(0) == false);
}

TEST_CASE("Vérification getComponent"){
    
    struct Position{
        float x;
        float y;
    };
   ComponentManager CM = ComponentManager();
    
    CM.addComponent<Position>(0, {75, 75});

    CHECK(CM.getComponent<Position>(0).x == 75);
}

TEST_CASE("Vérification hasComponent avec un component inéxistant"){
    
    struct Position{
        float x;
        float y;
    };
   ComponentManager CM = ComponentManager();
    
    CHECK(CM.hasComponent<Position>(0) == false);
}

