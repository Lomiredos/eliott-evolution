#pragma once
#include <vector>


enum class DetectedType{
    Nothing,
    Food,
    Wall
};

struct Ray{
    float maxRange;  
    float rotation;   
    float distance;   
    DetectedType detected;
};

struct Vision {
    std::vector<Ray> rays;
};