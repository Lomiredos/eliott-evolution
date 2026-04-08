#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include "evolution/Simulation.hpp"
#include "evolution/Species/Species.hpp"
#include "evolution/Species/SpeciesSerializer.hpp"

class App {

private:
    sf::RenderWindow m_window;
    sf::Font m_font;

    bool m_onMainMenu = true;
    Species* m_currentSpecies = nullptr;
    std::vector<Species> m_speciesList;
    std::vector<sf::FloatRect> m_speciesCardBounds;

    // une paire thread + mutex par espèce en cours de simulation
    struct SimEntry {
        std::thread thread;
        std::unique_ptr<Simulation> sim;
        std::mutex speciesMutex;
    };
    std::vector<std::unique_ptr<SimEntry>> m_simEntries;

public:
    App();
    ~App();
    void run();

private:
    void handleEvents();
    void update(float _dt);
    void draw();
    void loadSpecies();
    void drawMainMenu();
    void drawSpeciesDetail();

    void launchSimulation(Species& species);
    void stopAllSimulations();

    void drawText(const std::string& _str, sf::Vector2f _pos, int _size, sf::Color _color = sf::Color::White);
    void drawRect(sf::Vector2f _pos, sf::Vector2f _size, sf::Color _color);
    void drawGraph();
};
