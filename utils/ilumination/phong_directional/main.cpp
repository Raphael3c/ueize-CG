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