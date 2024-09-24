#ifndef COMMON_H
#define COMMON_H

#include <unordered_map>
#include <glm/glm.hpp>
#include <algorithm> 

struct StreetPoints {
    std::unordered_map<float, glm::vec3> points;
};

struct SelectedPoint {
    int streetIndex;
    float pointPercentage;
};

struct Light {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;  // Para luz direcional
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct Camera {
    glm::vec3 position;
};

#endif // COMMON_H
