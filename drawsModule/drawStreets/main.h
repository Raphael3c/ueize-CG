#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glut.h>
#include <unordered_map>
#include <vector>

#include "../../common.h"

void drawStreets(std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, Camera camera, Light light, Light light_spot, unsigned int texId1, unsigned int texId2);

#endif // MAIN_H
