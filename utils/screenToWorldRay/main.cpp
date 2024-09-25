#include "main.h"

glm::vec3 screenToWorldRay(int mouseX, int mouseY, int windowWidth, int windowHeight, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    float x = (2.0f * mouseX) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / windowHeight; 
    float z = 1.0f;

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);  

    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    
    return rayWorld;
}
