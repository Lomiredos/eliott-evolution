#include "evolution/Simulation.hpp"
#include <cmath>
#include <random>

Simulation::Simulation(Species& species, std::mutex& mutex)
    : m_species(species)
    , m_speciesMutex(mutex)
{
}

void Simulation::run() {
    // fenetre creee dans ce thread
    m_window.create(sf::VideoMode({1280u, 720u}), "Evolution - " + m_species.name);
    m_window.setFramerateLimit(60);
    setupSystems();
    spawnEntities();

    sf::Clock clock;
    while (m_window.isOpen() && m_running) {
        float dt = std::min(clock.restart().asSeconds(), 0.05f);
        handleEvents();
        update(dt);
        draw();
    }
    m_window.close();
    m_running = false;
}

void Simulation::setupSystems() {
    auto foodSys   = m_world.registerSystem<FoodSystem>();
    auto wallSys   = m_world.registerSystem<WallSystem>();
    auto visionSys = m_world.registerSystem<VisionSystem>();
    visionSys->init(foodSys, wallSys);
    auto moveSys   = m_world.registerSystem<MovementSystem>();
    auto neuralSys = m_world.registerSystem<NeuralSystem>();
    auto simSys    = m_world.registerSystem<SimulationSystem>();
    simSys->init(foodSys);
    simSys->setGeneration(m_species.generation);
    simSys->initHistory(m_species.fitnessMax, m_species.fitnessMin);

    m_sys = {moveSys, visionSys, neuralSys, simSys, foodSys, wallSys};

    ee::ecs::Signature sig;
    sig.set(ee::ecs::getComponentID<Transform>());
    sig.set(ee::ecs::getComponentID<Motion>());
    m_world.setSystemSignature<MovementSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<Transform>());
    sig.set(ee::ecs::getComponentID<Motion>());
    sig.set(ee::ecs::getComponentID<Vision>());
    m_world.setSystemSignature<VisionSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<NeuralNetwork>());
    sig.set(ee::ecs::getComponentID<Motion>());
    sig.set(ee::ecs::getComponentID<Vision>());
    m_world.setSystemSignature<NeuralSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<Food>());
    m_world.setSystemSignature<FoodSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<Wall>());
    m_world.setSystemSignature<WallSystem>(sig);

    sig.reset();
    sig.set(ee::ecs::getComponentID<NeuralNetwork>());
    m_world.setSystemSignature<SimulationSystem>(sig);
}

void Simulation::spawnEntities() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(50.f, 1230.f);
    std::uniform_real_distribution<float> distY(50.f, 670.f);
    std::uniform_real_distribution<float> distDir(0.f, 360.f);

    for (int i = 0; i < 30; i++) {
        auto food = m_world.createEntity();
        m_world.addComponent(food, Transform{sf::Vector2f(distX(rng), distY(rng))});
        m_world.addComponent(food, Food{});
    }

    for (int i = 0; i < 50; i++) {
        auto creature = m_world.createEntity();
        m_world.addComponent(creature, Transform{sf::Vector2f(distX(rng), distY(rng))});
        m_world.addComponent(creature, Motion{distDir(rng), m_species.rotationSpeed, 0.f, m_species.maxSpeed});
        m_world.addComponent(creature, Vitals{m_species.maxHealth, m_species.maxHealth, 0.f});
        std::vector<Ray> rays;
        for (int j = -2; j <= 2; j++)
            rays.push_back(Ray{100.f, (float)(j * 30), 100.f, DetectedType::Nothing});
        m_world.addComponent(creature, Vision{rays});
        m_world.addComponent(creature, m_sys.sim->createRandom(m_species.topology));
    }

    for (int x = 0; x < 1280; x += 20) {
        auto w = m_world.createEntity();
        m_world.addComponent(w, Transform{sf::Vector2f((float)x, 0.f)});
        m_world.addComponent(w, Wall{});
        w = m_world.createEntity();
        m_world.addComponent(w, Transform{sf::Vector2f((float)x, 720.f)});
        m_world.addComponent(w, Wall{});
    }
    for (int y = 0; y < 720; y += 20) {
        auto w = m_world.createEntity();
        m_world.addComponent(w, Transform{sf::Vector2f(0.f, (float)y)});
        m_world.addComponent(w, Wall{});
        w = m_world.createEntity();
        m_world.addComponent(w, Transform{sf::Vector2f(1280.f, (float)y)});
        m_world.addComponent(w, Wall{});
    }
}

void Simulation::handleEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { m_running = false; m_window.close(); }
        if (event->is<sf::Event::KeyPressed>()) {
            if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space)
                m_timeScale = (m_timeScale == 1.f) ? 5.f : 1.f;
        }
    }
}

void Simulation::update(float dt) {
    m_world.flushDestructions();
    float scaledDt = dt * m_timeScale;

    int genBefore = m_sys.sim->getGeneration();
    m_sys.vision->update(m_world, scaledDt);
    m_sys.neural->update(m_world, scaledDt);
    m_sys.move->update(m_world, scaledDt);
    m_sys.sim->update(m_world, scaledDt);

    if (m_sys.sim->getGeneration() > genBefore) {
        std::lock_guard<std::mutex> lock(m_speciesMutex);
        m_species.generation = m_sys.sim->getGeneration();
        m_species.fitnessMax  = m_sys.sim->getFitnessMax();
        m_species.fitnessMin  = m_sys.sim->getFitnessMin();
        m_species.bestBrain   = m_sys.sim->getBestBrain();
    }
}

void Simulation::draw() {
    m_window.clear();

    for (auto id : m_sys.food->m_entities) {
        auto& t = m_world.getComponent<Transform>(id);
        sf::CircleShape c(8.f);
        c.setFillColor(sf::Color::Green);
        c.setPosition(t.position - sf::Vector2f(8.f, 8.f));
        m_window.draw(c);
    }

    for (auto id : m_sys.wall->m_entities) {
        auto& t = m_world.getComponent<Transform>(id);
        sf::RectangleShape r({20.f, 20.f});
        r.setFillColor(sf::Color(100, 100, 100));
        r.setPosition(t.position - sf::Vector2f(20.f, 20.f));
        m_window.draw(r);
    }

    for (auto id : m_sys.move->m_entities) {
        auto& transform = m_world.getComponent<Transform>(id);
        auto& motion    = m_world.getComponent<Motion>(id);
        auto& vision    = m_world.getComponent<Vision>(id);

        sf::CircleShape c(12.f);
        c.setFillColor(sf::Color::White);
        c.setPosition(transform.position - sf::Vector2f(12.f, 12.f));
        m_window.draw(c);

        float D = motion.direction * (M_PI / 180.f);
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = transform.position;
        line[0].color    = sf::Color::White;
        line[1].position = transform.position + sf::Vector2f(cos(D)*100.f, sin(D)*100.f);
        line[1].color    = sf::Color::White;
        m_window.draw(line);

        for (auto& ray : vision.rays) {
            float Dr = (motion.direction + ray.rotation) * (M_PI / 180.f);
            sf::Color col = (ray.detected == DetectedType::Food) ? sf::Color::Green :
                            (ray.detected == DetectedType::Wall)  ? sf::Color::Red :
                                                                    sf::Color(100,100,100);
            sf::VertexArray rl(sf::PrimitiveType::Lines, 2);
            rl[0].position = transform.position; rl[0].color = col;
            rl[1].position = transform.position + sf::Vector2f(cos(Dr)*ray.distance, sin(Dr)*ray.distance);
            rl[1].color = col;
            m_window.draw(rl);
        }
    }

    m_window.display();
}
