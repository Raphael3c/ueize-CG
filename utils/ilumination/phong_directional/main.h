#ifndef MAIN_H
#define MAIN_H

#include <glm/glm.hpp>
#include "../../../common.h"

glm::vec3 shading(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera);
glm::vec3 shading_spot(const glm::vec3& point, const glm::vec3& normal, const Light& light, const Material& material, const Camera& camera, float cutoffAngle);

#endif  // MAIN_H
