#include "main.h"

void calculateWorldCoordinates(int streetIndex, glm::mat4 transformMatrix, float length, std::unordered_map<int, StreetPoints>& worldCoordinates) {
    float step = length / 4;

    std::vector<glm::vec3> localPoints = {
        glm::vec3(-length / 2, 0.2f, 0.0f),
        glm::vec3(-length / 2 + step, 0.2f, 0.0f),
        glm::vec3(-length / 2 + step * 2, 0.2f, 0.0f),
        glm::vec3(-length / 2 + step * 3, 0.2f, 0.0f),
        glm::vec3(-length / 2 + step * 4, 0.2f, 0.0f)
    };

    StreetPoints streetPoints;
    std::vector<float> stepPercentages = {0, 25, 50, 75, 100};

    for (int i = 0; i < localPoints.size(); ++i) {
        glm::vec4 transformedPoint = transformMatrix * glm::vec4(localPoints[i], 1.0f);
        streetPoints.points[stepPercentages[i]] = glm::vec3(transformedPoint);
    }

    worldCoordinates[streetIndex] = streetPoints;
}
