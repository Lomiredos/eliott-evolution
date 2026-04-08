#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>

#include "ecs/World.hpp"
#include "evolution/Species/Species.hpp"

#include "evolution/Components/Motion.hpp"
#include "evolution/Components/NeuralNetwork.hpp"
#include "evolution/Components/Transform.hpp"
#include "evolution/Components/Vitals.hpp"
#include "evolution/Components/Tags.hpp"
#include "evolution/Components/Vision.hpp"

#include "evolution/Systems/MovementSystem.hpp"
#include "evolution/Systems/NeuralSystem.hpp"
#include "evolution/Systems/SimulationSystem.hpp"
#include "evolution/Systems/TagsSystems.hpp"
#include "evolution/Systems/VisionSystem.hpp"

struct SimSystems {
    std::shared_ptr<MovementSystem> move;
    std::shared_ptr<VisionSystem>   vision;
    std::shared_ptr<NeuralSystem>   neural;
    std::shared_ptr<SimulationSystem> sim;
    std::shared_ptr<FoodSystem>     food;
    std::shared_ptr<WallSystem>     wall;
};

class Simulation {
private:
    sf::RenderWindow    m_window;
    ee::ecs::World      m_world;
    SimSystems          m_sys;
    Species&            m_species;
    std::mutex&         m_speciesMutex;
    float               m_timeScale = 5.f;
    std::atomic<bool>   m_running{true};

public:
    Simulation(Species& species, std::mutex& mutex);

    void run();   // boucle complete — a appeler depuis un thread
    void stop() { m_running = false; }
    bool isRunning() const { return m_running; }

private:
    void handleEvents();
    void update(float dt);
    void draw();
    void setupSystems();
    void spawnEntities();
};
