#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ecs/EntityManager.hpp"

using namespace ee::ecs;

TEST_CASE("Creation d'une entité"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    CHECK(firstid == 0);
}

TEST_CASE("Destruction d'une entité"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    EM.DestroyEntity(firstid);
    CHECK(EM.isAlive(firstid) == false);
    
}
TEST_CASE("Réutilisation des IDs"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    EM.DestroyEntity(firstid);
    EntityID  secondeId = EM.createEntity();
    CHECK(firstid == secondeId);
}

TEST_CASE("Vérification si une entité est vivante"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    CHECK(EM.isAlive(firstid));
}


TEST_CASE("Vérification si le guard de DestroyEntity marche bien"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    EM.DestroyEntity(firstid);
    EM.DestroyEntity(firstid);
    CHECK(EM.getEntityCount() == 0);
}

TEST_CASE("Vérification que le suivie du nombre des entité est bon"){
    EntityManager EM = EntityManager();
    EntityID firstid = EM.createEntity();
    EntityID seconid = EM.createEntity();
    CHECK(EM.getEntityCount() == 2);

}