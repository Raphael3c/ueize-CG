#pragma once
#ifndef CREATE_STREET_H
#define CREATE_STREET_H

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <GL/glut.h>
#include <unordered_map>
#include <vector>

#include "../../common.h"

void createStreet(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, int streetIndex, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Material& material, const Camera& camera);

void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex, std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Material& material, const Camera& camera);

#endif // CREATE_STREET_H
