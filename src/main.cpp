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

void drawText(sf::RenderWindow& win, const sf::Font& font, 
              const std::string& str, float x, float y, sf::Color color = sf::Color::White) {
    sf::Text text(font);
    text.setString(str);
    text.setCharacterSize(18);
    text.setFillColor(color);
    text.setPosition({x, y});
    win.draw(text);
}



Systems setUpSystem(ee::ecs::World& world){
    auto foodSys = world.registerSystem<FoodSystem>();
    auto wallSys = world.registerSystem<WallSystem>();
    auto visionSys = world.registerSystem<VisionSystem>();
    visionSys->init(foodSys, wallSys);
    auto moveSys = world.registerSystem<MovementSystem>();
    auto neuralSys = world.registerSystem<NeuralSystem>();
    auto simSys = world.registerSystem<SimulationSystem>();
    simSys->init(foodSys);

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

    sig.reset();
sig.set(ee::ecs::getComponentID<NeuralNetwork>());
world.setSystemSignature<SimulationSystem>(sig);


    return sys;
}

void createEntity(ee::ecs::World& world, std::shared_ptr<SimulationSystem> simSys){

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(50.f, 1230.f);
    std::uniform_real_distribution<float> distY(50.f, 670.f);
    std::uniform_real_distribution<float> distDir(0.f, 360.f);


    //Foods
    for (int i = 0; i < 10; i++){
        ee::ecs::EntityID food = world.createEntity();
        world.addComponent(food, Transform{ sf::Vector2f(distX(rng), distY(rng)) });
        world.addComponent(food, Food{});
    }

    //Creature
    for (int i = 0; i < 20; i++){
        ee::ecs::EntityID creature = world.createEntity();
        world.addComponent(creature, Transform{ sf::Vector2f(distX(rng), distY(rng)) });
        world.addComponent(creature, Motion{0.f, 90.f, 0.f, 150.f });
        world.addComponent(creature, Vitals{ 100.f, 100.f, 0.f });
        std::vector<Ray> rays;
        for (int i = -2; i <= 2; i++){
            rays.push_back(Ray{100.f, (float)(i * 30), 100.f, DetectedType::Nothing});
        }
        world.addComponent(creature, Vision{rays});
        world.addComponent(creature, simSys->createRandom({5, 8, 2}));
    }

    //Walls
    //Haut
    for (int x = 0; x < 1280; x+= 20){
        ee::ecs::EntityID wall = world.createEntity();
        world.addComponent(wall, Transform{ sf::Vector2f(x, 0) });
        world.addComponent(wall, Wall{});
    }
    //Bas
    for (int x = 0; x < 1280; x+= 20){
        ee::ecs::EntityID wall = world.createEntity();
        world.addComponent(wall, Transform{ sf::Vector2f(x, 720) });
        world.addComponent(wall, Wall{});
    }
    //Gauche
    for (int y = 0; y < 720; y+= 20){
        ee::ecs::EntityID wall = world.createEntity();
        world.addComponent(wall, Transform{ sf::Vector2f(0, y) });
        world.addComponent(wall, Wall{});
    }

    //Droite
    for (int y = 0; y < 720; y+= 20){
        ee::ecs::EntityID wall = world.createEntity();
        world.addComponent(wall, Transform{ sf::Vector2f(1280, y) });
        world.addComponent(wall, Wall{});
    }



}



int main()
{

    float timeScale = 5.f;
    //sf part
    sf::RenderWindow window(sf::VideoMode({1280u, 720u}), "Evolution");
    window.setFramerateLimit(60);

    sf::RenderWindow statsWindow(sf::VideoMode({400u, 600u}), "Stats");
    sf::Font font;
    font.openFromFile("C:/Windows/Fonts/arial.ttf");

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
    while (const std::optional event = statsWindow.pollEvent()) {
    if (event->is<sf::Event::Closed>()) statsWindow.close();
    }

	dt = std::min(clock.restart().asSeconds(), 0.05f);

    //clear des entité morte
    world.flushDestructions();


    float scaledDt = dt * timeScale;
    
    //update
    sys.vision->update(world, scaledDt);
    sys.neural->update(world, scaledDt);
    sys.move->update(world, scaledDt);
    sys.sim->update(world, scaledDt);


    //draw
    window.clear();


    for (ee::ecs::EntityID id : sys.food->m_entities){
        Transform transform = world.getComponent<Transform>(id);
        sf::CircleShape circle(8.f);
        circle.setFillColor(sf::Color::Green);
        circle.setPosition(transform.position - sf::Vector2f(8.f, 8.f));
        window.draw(circle);
    }

    for (ee::ecs::EntityID id : sys.wall->m_entities){
        Transform transform = world.getComponent<Transform>(id);
        sf::RectangleShape rect({20., 20.});
        rect.setFillColor(sf::Color(100, 100, 100));
        rect.setPosition(transform.position - sf::Vector2f(20.f, 20.f));
        window.draw(rect);
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


    statsWindow.clear();


    std::string text = "Generation: " + std::to_string(sys.sim->getGeneration());
    drawText(statsWindow, font, text, 40, 50);
    
    text = "Time Factors :" + std::to_string(timeScale);
    drawText(statsWindow, font, text, 40, 100);

    text = "Time :" + std::to_string(sys.sim->getTimer()) +"/" +std::to_string(sys.sim->getGenerationTime());
    drawText(statsWindow, font, text, 40, 150);


    statsWindow.display();



    }
    return 0;
}