#ifndef MAIN_H
#define MAIN_H

#include <glm/glm.hpp>
#include "../../../common.h"

// Função para calcular a iluminação Phong com luz direcional
glm::vec3 shading(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera);

#endif  // MAIN_H
