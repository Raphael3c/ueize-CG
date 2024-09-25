#include "main.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>  // Para std::max
#include <GL/gl.h>

// Função que implementa a iluminação Phong com luz direcional
glm::vec3 shading(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera) {
    // Componente ambiente
    glm::vec3 shadeAmbient = light.ambient * material.ambient;

    // Normalizar a direção da luz
    glm::vec3 l = glm::normalize(-light.direction);  // Direção oposta à luz
    glm::vec3 n = glm::normalize(normal);

    // Cálculo da componente difusa
    glm::vec3 shadeDiffuse = light.diffuse * material.diffuse * glm::max(0.0f, glm::dot(l, n));

    // Cálculo da componente especular
    glm::vec3 v = glm::normalize(camera.position - point);  // Vetor da câmera até o ponto
    glm::vec3 r = ((2 * glm::dot(n, l)) * n) - l;  // Vetor refletido
    glm::vec3 shadeSpecular = light.specular * material.specular * glm::pow(glm::max(0.0f, glm::dot(v, r)), material.shininess);

    // Somar as componentes: ambiente, difusa e especular
    return shadeAmbient + shadeDiffuse + shadeSpecular;
}


glm::vec3 shading_spot(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera, float cutoffAngle) {
    // Lógica da iluminação spot
    glm::vec3 shadeAmbient = light.ambient * material.ambient; // Componente ambiente da luz
    
    glm::vec3 lightDir = glm::normalize(light.position - point); // Direção da luz em relação ao ponto
    glm::vec3 normalVec = glm::normalize(normal); // Normal do ponto

    // Cálculo do efeito spot
    float spotEffect = glm::dot(glm::normalize(light.direction), -lightDir);

    // Verifica se o ponto está dentro do cone da luz spot
    if (spotEffect > glm::cos(glm::radians(cutoffAngle))) {
        // Cálculo da componente difusa (luz espalhada pela superfície)
        glm::vec3 shadeDiffuse = light.diffuse * material.diffuse * glm::max(0.0f, glm::dot(lightDir, normalVec));

        // Cálculo da componente especular (reflexos brilhantes)
        glm::vec3 viewDir = glm::normalize(camera.position - point); // Direção da câmera (ponto de vista)
        glm::vec3 reflectDir = glm::reflect(-lightDir, normalVec); // Direção refletida
        glm::vec3 shadeSpecular = light.specular * material.specular * glm::pow(glm::max(0.0f, glm::dot(viewDir, reflectDir)), material.shininess);
        
        // Soma os componentes ambiente, difuso e especular
        return shadeAmbient + shadeDiffuse + shadeSpecular;
    }

    // Se o ponto está fora do cone da luz spot, apenas o componente ambiente é aplicado
    return shadeAmbient;
}