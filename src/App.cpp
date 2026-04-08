#include "evolution/App.hpp"
#include <filesystem>
#include <algorithm>

App::App() : m_window(sf::VideoMode({600u, 800u}), "Evolution Generator"){
    m_font.openFromFile("C:/Windows/Fonts/arial.ttf");
    loadSpecies();
}

App::~App() {
    stopAllSimulations();
}

void App::stopAllSimulations() {
    for (auto& entry : m_simEntries)
        entry->sim->stop();
    for (auto& entry : m_simEntries)
        if (entry->thread.joinable())
            entry->thread.join();
    m_simEntries.clear();
}

void App::launchSimulation(Species& species) {
    auto entry = std::make_unique<SimEntry>();
    entry->sim = std::make_unique<Simulation>(species, entry->speciesMutex);
    Simulation* simPtr = entry->sim.get();
    entry->thread = std::thread([simPtr]() { simPtr->run(); });
    m_simEntries.push_back(std::move(entry));
}

void App::run(){
    sf::Clock clock;
    float dt;

    while(m_window.isOpen()) {
        dt = std::min(clock.restart().asSeconds(), 0.05f);
        handleEvents();
        update(dt);
        draw();

        // nettoyer les simulations terminées
        for (auto& entry : m_simEntries) {
            if (!entry->sim->isRunning() && entry->thread.joinable())
                entry->thread.join();
        }
        m_simEntries.erase(
            std::remove_if(m_simEntries.begin(), m_simEntries.end(),
                [](const auto& e) { return !e->sim->isRunning() && !e->thread.joinable(); }),
            m_simEntries.end()
        );
    }
    stopAllSimulations();
}

void App::handleEvents(){
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();

        if (event->is<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(m_window));

            if (m_onMainMenu) {
                for (int i = 0; i < (int)m_speciesCardBounds.size(); i++) {
                    if (m_speciesCardBounds[i].contains(mouse)) {
                        m_currentSpecies = &m_speciesList[i];
                        m_onMainMenu = false;
                        break;
                    }
                }
            } else {
                sf::FloatRect backBtn({10.f, 10.f}, {80.f, 35.f});
                if (backBtn.contains(mouse)) {
                    m_onMainMenu = true;
                    m_currentSpecies = nullptr;
                    return;
                }

                sf::FloatRect btnBounds({10.f, 680.f}, {580.f, 100.f});
                if (btnBounds.contains(mouse) && m_currentSpecies != nullptr) {
                    if (!m_currentSpecies->isEvolving) {
                        m_currentSpecies->isEvolving = true;
                        launchSimulation(*m_currentSpecies);
                    } else {
                        m_currentSpecies->isEvolving = false;
                        for (auto& entry : m_simEntries)
                            entry->sim->stop();
                    }
                }
            }
        }
    }
}

void App::update(float _dt){}

void App::draw(){
    m_window.clear();
    if (m_onMainMenu) drawMainMenu();
    else              drawSpeciesDetail();
    m_window.display();
}

void App::drawMainMenu(){
    const float H = 800.f;
    drawText("Evolution Generator", {80.f, 25.f}, 36);
    drawText("by Lomiredo", {170.f, 68.f}, 24);

    if (m_speciesList.empty()) {
        drawRect({50.f, 160.f}, {500.f, 250.f}, sf::Color(70, 70, 70));
        drawText("Create new species", {130.f, 270.f}, 28);
        drawRect({50.f, 450.f}, {500.f, 250.f}, sf::Color(70, 70, 70));
        drawText("Import species", {165.f, 560.f}, 28);
    } else {
        float y = 150.f;
        m_speciesCardBounds.clear();
        for (int i = 0; i < (int)m_speciesList.size(); i++) {
            Species& s = m_speciesList[i];
            sf::Color cardColor = s.isEvolving ? sf::Color(50, 90, 50) : sf::Color(60, 60, 60);
            drawRect({20.f, y}, {460.f, 80.f}, cardColor);
            m_speciesCardBounds.push_back(sf::FloatRect({20.f, y}, {460.f, 80.f}));
            drawRect({30.f, y + 10.f}, {60.f, 60.f}, sf::Color(90, 90, 90));
            drawText("img", {45.f, y + 30.f}, 12);
            drawText("-name : " + s.name, {105.f, y + 10.f}, 16);
            std::string state = s.isEvolving ? "evolving" : "waiting";
            drawText("-state : " + state, {105.f, y + 35.f}, 14);
            drawText("gen " + std::to_string(s.generation), {105.f, y + 55.f}, 13);
            y += 95.f;
        }
        drawRect({20.f, H - 120.f}, {200.f, 80.f}, sf::Color(70, 70, 70));
        drawText("Create species", {30.f, H - 95.f}, 20);
        drawRect({340.f, H - 120.f}, {200.f, 80.f}, sf::Color(70, 70, 70));
        drawText("Import species", {350.f, H - 95.f}, 20);
    }
}

void App::drawSpeciesDetail(){
    if (m_currentSpecies == nullptr) return;
    const float W = 600.f, H = 800.f;

    drawRect({10.f, 10.f}, {80.f, 35.f}, sf::Color(70, 70, 70));
    drawText("<- Back", {18.f, 16.f}, 16);
    drawText(m_currentSpecies->name, {10.f, 60.f}, 42);
    drawText("Generation : " + std::to_string(m_currentSpecies->generation), {10.f, 110.f}, 22);
    drawRect({10.f, 150.f}, {180.f, 180.f}, sf::Color(80, 80, 80));
    drawText("Image", {75.f, 230.f}, 16);
    drawText("Stats :", {210.f, 150.f}, 22);

    float sy = 185.f; float labelX = 220.f; float valueX = 420.f; int fs = 17;
    drawText("Speed :",        {labelX, sy}, fs); drawText(std::to_string((int)m_currentSpecies->maxSpeed),        {valueX, sy}, fs); sy += 30.f;
    drawText("Rotation spd :", {labelX, sy}, fs); drawText(std::to_string((int)m_currentSpecies->rotationSpeed),   {valueX, sy}, fs); sy += 30.f;
    drawText("Health :",       {labelX, sy}, fs); drawText(std::to_string((int)m_currentSpecies->maxHealth),       {valueX, sy}, fs); sy += 30.f;
    drawText("Hunger :",       {labelX, sy}, fs); drawText(std::to_string((int)m_currentSpecies->maxHunger),       {valueX, sy}, fs); sy += 30.f;
    drawText("Food type :",    {labelX, sy}, fs); drawText(m_currentSpecies->foodType,                              {valueX, sy}, fs);

    // Graphe fitness
    drawGraph();

    sf::Color btnColor = m_currentSpecies->isEvolving ? sf::Color(90, 50, 50) : sf::Color(50, 90, 50);
    drawRect({10.f, H - 120.f}, {W - 20.f, 100.f}, btnColor);
    std::string btnText = m_currentSpecies->isEvolving ? "Stop Evolution" : "Launch Evolution";
    drawText(btnText, {W / 2.f - 80.f, H - 82.f}, 26);
}

void App::drawText(const std::string& _str, sf::Vector2f _pos, int _size, sf::Color _color){
    sf::Text text(m_font);
    text.setString(_str); text.setCharacterSize(_size);
    text.setFillColor(_color); text.setPosition(_pos);
    m_window.draw(text);
}

void App::drawRect(sf::Vector2f _pos, sf::Vector2f _size, sf::Color _color){
    sf::RectangleShape rect(_size);
    rect.setFillColor(_color); rect.setPosition(_pos);
    m_window.draw(rect);
}

void App::drawGraph() {
    if (!m_currentSpecies) return;

    const auto& maxData = m_currentSpecies->fitnessMax;
    const auto& minData = m_currentSpecies->fitnessMin;
    if (maxData.empty()) {
        drawText("No data yet...", {20.f, 400.f}, 16, sf::Color(150,150,150));
        return;
    }

    // Zone du graphe
    const float gx = 20.f, gy = 380.f;
    const float gw = 560.f, gh = 270.f;

    // Fond
    drawRect({gx, gy}, {gw, gh}, sf::Color(40, 40, 40));

    // Trouver min/max global pour normaliser
    float globalMax = *std::max_element(maxData.begin(), maxData.end());
    float globalMin = *std::min_element(minData.begin(), minData.end());
    if (globalMax == globalMin) globalMax = globalMin + 1.f;

    // Labels axes
    drawText("Fitness", {gx, gy - 20.f}, 14, sf::Color(200,200,200));
    drawText(std::to_string((int)globalMax), {gx + 2.f, gy + 2.f}, 11, sf::Color(150,150,150));
    drawText(std::to_string((int)globalMin), {gx + 2.f, gy + gh - 16.f}, 11, sf::Color(150,150,150));
    drawText("Gen", {gx + gw - 30.f, gy + gh + 2.f}, 12, sf::Color(200,200,200));

    int n = (int)maxData.size();
    if (n < 2) {
        drawText("Need 2+ generations", {gx + 10.f, gy + gh/2.f}, 14, sf::Color(150,150,150));
        return;
    }

    float stepX = gw / (float)(n - 1);

    // Dessiner les courbes avec VertexArray
    sf::VertexArray lineMax(sf::PrimitiveType::LineStrip, n);
    sf::VertexArray lineMin(sf::PrimitiveType::LineStrip, n);

    for (int i = 0; i < n; i++) {
        float px = gx + i * stepX;

        float normMax = 1.f - (maxData[i] - globalMin) / (globalMax - globalMin);
        float normMin = 1.f - (minData[i] - globalMin) / (globalMax - globalMin);

        lineMax[i].position = {px, gy + normMax * gh};
        lineMax[i].color    = sf::Color(80, 200, 80);   // vert = max

        lineMin[i].position = {px, gy + normMin * gh};
        lineMin[i].color    = sf::Color(200, 80, 80);   // rouge = min
    }

    m_window.draw(lineMax);
    m_window.draw(lineMin);

    // Légende
    drawRect({gx + gw - 120.f, gy + 8.f},  {14.f, 14.f}, sf::Color(80, 200, 80));
    drawText("Max", {gx + gw - 100.f, gy + 6.f}, 13, sf::Color(80, 200, 80));
    drawRect({gx + gw - 120.f, gy + 28.f}, {14.f, 14.f}, sf::Color(200, 80, 80));
    drawText("Min", {gx + gw - 100.f, gy + 26.f}, 13, sf::Color(200, 80, 80));
}

void App::loadSpecies(){
    namespace fs = std::filesystem;
    const std::string folder = "../../species/";
    if (!fs::exists(folder)) return;
    for (const auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() != ".json") continue;
        try {
            m_speciesList.push_back(SpeciesSerializer::load(entry.path().string()));
        } catch (...) {}
    }
}
