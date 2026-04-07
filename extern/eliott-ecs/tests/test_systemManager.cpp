#include <doctest/doctest.h>
#include "ecs/SystemManager.hpp"

using namespace ee::ecs;

class MovementSystem : public System {};

TEST_CASE("Vérification register de system"){
    SystemManager SM = SystemManager();

    CHECK(SM.registerSystem<MovementSystem>() != nullptr);
}
TEST_CASE("Vérification si les entités sont bien dans les systèmes"){
    SystemManager SM;
    auto system = SM.registerSystem<MovementSystem>();

    Signature systemSig;
    systemSig.set(0);
    systemSig.set(1);
    SM.setSignature<MovementSystem>(systemSig);

    Signature entitySig;
    entitySig.set(0);
    entitySig.set(1);
    SM.onEntitySignatureChanged(0, entitySig);

    CHECK(system->m_entities.contains(0));
}

TEST_CASE("Vérification qu'une entité sans la bonne signature n'est pas dans le système"){
    SystemManager SM;
    auto system = SM.registerSystem<MovementSystem>();

    Signature systemSig;
    systemSig.set(0);
    systemSig.set(1);
    SM.setSignature<MovementSystem>(systemSig);

    Signature entitySig;
    entitySig.set(0);  // seulement Position, manque Velocity
    SM.onEntitySignatureChanged(1, entitySig);

    CHECK(system->m_entities.contains(1) == false);
}

TEST_CASE("Vérification qu'une entité est bien retirer apres le onEntityDEstroyed"){
    SystemManager SM;
    auto system = SM.registerSystem<MovementSystem>();

    Signature systemSig;
    systemSig.set(0);
    systemSig.set(1);
    SM.setSignature<MovementSystem>(systemSig);

    Signature entitySig;
    entitySig.set(0); 
    entitySig.set(1);
    SM.onEntitySignatureChanged(1, entitySig);

    SM.onEntityDestroyed(1);

    CHECK(system->m_entities.contains(1) == false);
}