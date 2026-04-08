#include <SFML/Graphics.hpp>



#include "ecs/World.hpp"


#include "evolution/Components/Motion.hpp"
#include "evolution/Components/NeuralNetwork.hpp"
#include "evolution/Components/Transform.hpp"
#include "evolution/Components/Vitals.hpp"
#include "evolution/Components/Tags.hpp"

#include "evolution/Systems/MovementSystem.hpp"
#include "evolution/Systems/NeuralSystem.hpp"
#include "evolution/Systems/SimulationSystem.hpp"
#include "evolution/Systems/TagsSystems.hpp"
#include "evolution/Systems/VisionSystem.hpp"

struct Systems {
    std::shared_ptr<MovementSystem> move;
    std::shared_ptr<VisionSystem> vision;
    std::shared_ptr<NeuralSystem> neural;
    std::shared_ptr<SimulationSystem> sim;
    std::shared_ptr<FoodSystem> food;
    std::shared_ptr<WallSystem> wall;
};


Systems setUpSystem(ee::ecs::World& world){
    auto foodSys = world.registerSystem<FoodSystem>();
    auto wallSys = world.registerSystem<WallSystem>();
    auto visionSys = world.registerSystem<VisionSystem>();
    visionSys->init(foodSys, wallSys);
    auto moveSys = world.registerSystem<MovementSystem>();
    auto neuralSys = world.registerSystem<NeuralSystem>();
    auto simSys = world.registerSystem<SimulationSystem>();

Systems sys{moveSys, visionSys, neuralSys, simSys, foodSys, wallSys};

    ee::ecs::Signature sig;
    sig.set(ee::ecs::getComponentID<Transform>());
    sig.set(ee::ecs::getComponentID<Motion>());
    world.setSystemSignature<MovementSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<Transform>());
    sig.set(ee::ecs::getComponentID<Motion>());
    sig.set(ee::ecs::getComponentID<Vision>());
    world.setSystemSignature<VisionSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<NeuralNetwork>());
    sig.set(ee::ecs::getComponentID<Motion>());
    sig.set(ee::ecs::getComponentID<Vision>());
    world.setSystemSignature<NeuralSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<Food>());
    world.setSystemSignature<FoodSystem>(sig);

        sig.reset();
    sig.set(ee::ecs::getComponentID<Wall>());
    world.setSystemSignature<WallSystem>(sig);


    return sys;
}

void createEntity(ee::ecs::World& world, std::shared_ptr<SimulationSystem> simSys){
ee::ecs::EntityID food = world.createEntity();
world.addComponent(food, Transform{ sf::Vector2f(400.f, 300.f) });
world.addComponent(food, Food{});

ee::ecs::EntityID creature = world.createEntity();
world.addComponent(creature, Transform{ sf::Vector2f(100.f, 100.f) });
world.addComponent(creature, Motion{ 0.f, 90.f, 0.f, 150.f });
world.addComponent(creature, Vitals{ 100.f, 100.f, 0.f });
std::vector<Ray> rays;
for (int i = -2; i <= 2; i++){
    rays.push_back(Ray{100.f, (float)(i * 30), 100.f, DetectedType::Nothing});
}
world.addComponent(creature, Vision{rays});
 world.addComponent(creature, simSys->createRandom({5, 8, 2}));

}



int main()
{
    //sf part
    sf::RenderWindow window =  sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Evolution");
    window.setFramerateLimit(60);
    sf::Clock clock;

    float dt;

    ee::ecs::World world;
    auto sys = setUpSystem(world);
    createEntity(world, sys.sim);



    while (window.isOpen())
    {
    //input
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
    }

	dt = std::min(clock.restart().asSeconds(), 0.05f);

    //update
    sys.vision->update(world, dt);
    sys.neural->update(world, dt);
    sys.move->update(world, dt);
    sys.sim->update(world, dt);

    //draw
    window.clear();


    for (ee::ecs::EntityID id : sys.food->m_entities){
        Transform transform = world.getComponent<Transform>(id);
        sf::CircleShape circle(8.f);
        circle.setFillColor(sf::Color::Green);
        circle.setPosition(transform.position - sf::Vector2f(8.f, 8.f));
        window.draw(circle);
    }

    for (ee::ecs::EntityID id : sys.move->m_entities){
        
        //corps
        Transform transform = world.getComponent<Transform>(id);
        sf::CircleShape circle(12.f);
        circle.setFillColor(sf::Color::White);
        circle.setPosition(transform.position - sf::Vector2f(12.f, 12.f));
        window.draw(circle);

        //direction
        Motion motion = world.getComponent<Motion>(id);
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        float D = (motion.direction) * (M_PI / 180.f);
        sf::Vector2f endPoint = transform.position + sf::Vector2f(
            cos(D) * 100,
            sin(D) * 100
        );

        line[0].position = transform.position;
        line[0].color    = sf::Color::White;
        line[1].position = endPoint;
        line[1].color    = sf::Color::White;
        window.draw(line);

        //vision
        Vision vision = world.getComponent<Vision>(id);

        for (Ray ray : vision.rays){
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            float D = (motion.direction + ray.rotation) * (M_PI / 180.f);
            sf::Vector2f endPoint = transform.position + sf::Vector2f(
                cos(D) * ray.distance,
                sin(D) * ray.distance
            );

            sf::Color color;
            switch (ray.detected){
                case DetectedType::Food: color = sf::Color::Green; break;
                case DetectedType::Wall: color = sf::Color::Red; break;
                case DetectedType::Nothing: color = sf::Color(100, 100, 100); break;

                default:color = sf::Color::Black; break;
            }
            line[0].position = transform.position;
            line[0].color = color;
            line[1].position = endPoint;
            line[1].color = color;
            window.draw(line);
        }

    }

    window.display();


    }



    return 0;
}