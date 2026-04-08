# eliott-evolution — Contexte de session

## But du projet
Simulation de neuroévolution en C++ avec SFML3 et ECS maison.
Des créatures apprennent à manger de la nourriture via un réseau de neurones qui évolue par sélection naturelle (approche A : générations fixes, les meilleures se reproduisent).

## Stack technique
- C++20, SFML 3.0.2, vcpkg (triplet x64-mingw-static)
- ECS maison : `extern/eliott-ecs` (submodule git)
- CMake + MinGW, VS Code avec GDB (C:/msys64/mingw64/bin/gdb.exe)
- Repo : https://github.com/Lomiredos/eliott-evolution

## Commandes utiles
```powershell
# Compiler
cd C:\Dev\eliott-engine-projects\eliott-evolution
cmake --preset mingw
cd build/mingw && mingw32-make

# Lancer
.\eliott-evolution.exe

# Git (toujours depuis la racine)
cd C:\Dev\eliott-engine-projects\eliott-evolution
git add . && git commit -m "..." && git push
```

## Architecture ECS
- `World` : point d'entrée central (createEntity, addComponent, registerSystem)
- `EntityManager` : crée/détruit les entités avec recyclage d'IDs
- `ComponentManager` : stockage typé par type_index
- `System` : classe de base avec `m_entities` (set d'IDs) et `update(World&, float dt)`
- Signatures : `std::bitset<64>` — définit quels composants un système requiert
- Pas besoin d'enregistrer les composants manuellement : `getComponentID<T>()` s'en occupe

## Composants implémentés
| Fichier | Struct | Données |
|---|---|---|
| Transform.hpp | Transform | sf::Vector2f position |
| Motion.hpp | Motion | float direction, rotationSpeed, speed, maxSpeed |
| Vitals.hpp | Vitals | float health, maxHealth, hunger |
| Vision.hpp | Ray + Vision | maxRange, rotation, distance, DetectedType + vector<Ray> rays |
| NeuralNetwork.hpp | NeuralNetwork | vector<vector<float>> weights, biases + float fitness |
| Tags.hpp | Food, Wall | structs vides (marqueurs) |

## Systèmes implémentés
| Fichier | Rôle |
|---|---|
| MovementSystem.hpp | Met à jour position via direction+speed+dt |
| VisionSystem.hpp | Raycasting — détecte Food/Wall dans chaque rayon |
| NeuralSystem.hpp | Forward pass — rayons → réseau → Motion |
| TagsSystems.hpp | FoodSystem + WallSystem (vides, servent de registres d'entités) |
| SimulationSystem.hpp | createRandom(topology) — génère un NeuralNetwork aléatoire |

## Points importants
- Angles en DEGRÉS dans Motion et Ray, conversion en radians dans VisionSystem uniquement
- forward() dans NeuralSystem : inputs → couches → tanh → outputs
- rayToFloat() : Nothing=0, Food=+1, Wall=-1, pondéré par proximité (1 - dist/maxRange)
- Poids indexés à plat : weights[i][j * nbEntrées + k]
- SimulationSystem a m_rng (mt19937) et m_dist (-1, 1) comme membres privés

## État actuel — CE QUI RESTE À FAIRE
### Prochaine étape immédiate : assembler le main.cpp
```cpp
// Ordre à suivre dans main() :
// 1. Créer ee::ecs::World world
// 2. Enregistrer les systèmes :
//    auto foodSys = world.registerSystem<FoodSystem>();
//    auto wallSys = world.registerSystem<WallSystem>();
//    auto visionSys = world.registerSystem<VisionSystem>();
//    visionSys->init(foodSys, wallSys);
//    auto moveSys = world.registerSystem<MovementSystem>();
//    auto neuralSys = world.registerSystem<NeuralSystem>();
//    auto simSys = world.registerSystem<SimulationSystem>();
//
// 3. Définir les signatures (world.setSystemSignature<T>(sig))
//    — Signature = bitset où chaque bit = un composant
//    — Utiliser getComponentID<T>() pour obtenir l'index du bit
//
// 4. Créer entités de test :
//    — quelques Food avec Transform + tag Food
//    — quelques Wall avec Transform + tag Wall
//    — une créature avec Transform + Motion + Vision + NeuralNetwork + Vitals
//
// 5. Boucle SFML : update systèmes + rendu basique (cercles colorés)
```

### Suite après le main :
- Rendu visuel : dessiner créatures (cercles) + rayons de vision + nourriture
- SimulationSystem.update() : logique de génération (timer, fin de gen, reproduction)
- Mutation des poids pour la reproduction (clone + bruit aléatoire)
- Graphes d'évolution (fitness par génération)
- Quadtree pour optimiser VisionSystem (déjà disponible dans eliott-engine)
- Mode B : simulation continue (reproduction naturelle sans générations fixes)
- Prédateurs (même architecture, paramètres différents)

## Questions en suspens
- Topologie du réseau : à décider (ex: {5, 8, 2} = 5 rayons, 8 cachés, 2 sorties)
- Nombre de créatures par génération
- Durée d'une génération (en secondes)
- Pourcentage des meilleures qui se reproduisent
