#ifndef SCREEN_TO_WORLD_RAY_H
#define SCREEN_TO_WORLD_RAY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Declaração da função
glm::vec3 screenToWorldRay(int mouseX, int mouseY, int windowWidth, int windowHeight, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

#endif  // SCREEN_TO_WORLD_RAY_H
