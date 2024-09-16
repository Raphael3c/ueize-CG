#ifndef COMMON_H
#define COMMON_H

#include <unordered_map>
#include <glm/glm.hpp>

struct StreetPoints {
    std::unordered_map<float, glm::vec3> points;
};

struct SelectedPoint {
    int streetIndex;
    float pointPercentage;
};

#endif // COMMON_H
