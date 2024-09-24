#include "main.h"

// Função que converte coordenadas de tela para um raio no mundo
glm::vec3 screenToWorldRay(int mouseX, int mouseY, int windowWidth, int windowHeight, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // Coordenadas normalizadas entre -1 e 1
    float x = (2.0f * mouseX) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / windowHeight; // Coordenadas Y invertidas
    float z = 1.0f; // Far plane

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Transformar de clip-space para eye-space
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);  // Definir z=-1, w=0 para o vetor direcional

    // Transformar de eye-space para world-space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    
    return rayWorld;
}
