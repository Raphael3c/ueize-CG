#include "main.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>  // Para std::max
#include <GL/gl.h>

glm::vec3 shading(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera) {
    glm::vec3 shadeAmbient = light.ambient * material.ambient;

    glm::vec3 l = glm::normalize(-light.direction); 
    glm::vec3 n = glm::normalize(normal);

    glm::vec3 shadeDiffuse = light.diffuse * material.diffuse * glm::max(0.0f, glm::dot(l, n));

    glm::vec3 v = glm::normalize(camera.position - point);  
    glm::vec3 r = ((2 * glm::dot(n, l)) * n) - l;  
    glm::vec3 shadeSpecular = light.specular * material.specular * glm::pow(glm::max(0.0f, glm::dot(v, r)), material.shininess);

    return shadeAmbient + shadeDiffuse + shadeSpecular;
}

glm::vec3 shading_spot(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera, float cutoffAngle) {
    glm::vec3 shadeAmbient = light.ambient * material.ambient;
    
    glm::vec3 lightDir = glm::normalize(light.position - point);
    glm::vec3 normalVec = glm::normalize(normal);
    
    float spotEffect = glm::dot(glm::normalize(light.direction), -lightDir);
    
    float intensity = glm::clamp((spotEffect - glm::cos(glm::radians(cutoffAngle))) / (1.0f - glm::cos(glm::radians(cutoffAngle))), 0.0f, 1.0f);
    
    if (intensity > 0.0f) {
        glm::vec3 shadeDiffuse = light.diffuse * material.diffuse * glm::max(0.0f, glm::dot(normalVec, lightDir)) * intensity;
        
        glm::vec3 viewDir = glm::normalize(camera.position - point);
        
        glm::vec3 reflectDir = glm::reflect(-lightDir, normalVec);
        
        glm::vec3 shadeSpecular = light.specular * material.specular * glm::pow(glm::max(0.0f, glm::dot(viewDir, reflectDir)), material.shininess) * intensity;
        
        return shadeAmbient + shadeDiffuse + shadeSpecular;
    }
    
    return shadeAmbient;
}
