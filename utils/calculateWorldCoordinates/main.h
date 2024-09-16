#pragma once
#ifndef CALCULATE_WORLD_COORDINATES_H
#define CALCULATE_WORLD_COORDINATES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <vector>

#include "../../common.h"

void calculateWorldCoordinates(int streetIndex, glm::mat4 transformMatrix, float length, std::unordered_map<int, StreetPoints>& worldCoordinates);

#endif // CALCULATE_WORLD_COORDINATES_H
