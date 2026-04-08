#pragma once

#include "evolution/Species/Species.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <stdexcept>

class SpeciesSerializer {
public:

    static void save(const Species& s, const std::string& path) {
        nlohmann::json j;

        j["name"]           = s.name;
        j["maxSpeed"]       = s.maxSpeed;
        j["rotationSpeed"]  = s.rotationSpeed;
        j["maxHealth"]      = s.maxHealth;
        j["maxHunger"]      = s.maxHunger;
        j["foodType"]       = s.foodType;
        j["topology"]       = s.topology;
        j["generation"]     = s.generation;
        j["fitnessMax"]     = s.fitnessMax;
        j["fitnessMin"]     = s.fitnessMin;

        j["bestBrain"]["weights"] = s.bestBrain.weights;
        j["bestBrain"]["biases"]  = s.bestBrain.biases;
        j["bestBrain"]["fitness"] = s.bestBrain.fitness;

        std::ofstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file for writing: " + path);
        file << j.dump(4);
    }

    static Species load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Cannot open file for reading: " + path);

        nlohmann::json j;
        file >> j;

        Species s;
        s.name          = j["name"];
        s.maxSpeed      = j["maxSpeed"];
        s.rotationSpeed = j["rotationSpeed"];
        s.maxHealth     = j["maxHealth"];
        s.maxHunger     = j["maxHunger"];
        s.foodType      = j["foodType"];
        s.topology      = j["topology"].get<std::vector<int>>();
        s.generation    = j["generation"];
        s.fitnessMax    = j["fitnessMax"].get<std::vector<float>>();
        s.fitnessMin    = j["fitnessMin"].get<std::vector<float>>();

        s.bestBrain.weights = j["bestBrain"]["weights"].get<std::vector<std::vector<float>>>();
        s.bestBrain.biases  = j["bestBrain"]["biases"].get<std::vector<std::vector<float>>>();
        s.bestBrain.fitness = j["bestBrain"]["fitness"];

        return s;
    }
};
