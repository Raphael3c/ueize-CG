#include "main.h"
#include "../../common.h"

extern std::unordered_map<int, StreetPoints> worldCoordinates;
extern std::vector<glm::vec3> lineCoordinates;
extern std::vector<int> findPathBetweenStreets(int startStreet, int endStreet, const std::unordered_map<int, std::vector<int>>& streetConnections);

glm::vec3 findClosestPointInStreet(int streetIndex, const glm::vec3& targetPoint) {
    float minDistance = FLT_MAX;
    glm::vec3 closestPoint;
    
    for (const auto& point : worldCoordinates[streetIndex].points) {
        float distance = glm::length(point.second - targetPoint);
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = point.second;
        }
    }
    return closestPoint;
}

SelectedPoint updateCurrentPoint(int streetIndex, const glm::vec3& newPoint) {
    auto it = std::find_if(
        worldCoordinates[streetIndex].points.begin(),
        worldCoordinates[streetIndex].points.end(),
        [&](const std::pair<float, glm::vec3>& p) { return p.second == newPoint; }
    );
    return {streetIndex, it->first};
}

void drawLineFollowingStreetTopology(const SelectedPoint& point1, const SelectedPoint& point2, std::unordered_map<int, std::vector<int>> streetConnections) {
    lineCoordinates.clear();  

    std::vector<int> streetPath = findPathBetweenStreets(point1.streetIndex, point2.streetIndex, streetConnections);

    SelectedPoint currentPoint = point1;

    for (size_t i = 0; i < streetPath.size(); ++i) {
        int currentStreetIndex = streetPath[i];
        glm::vec3 currentPointWorld = worldCoordinates[currentStreetIndex].points[currentPoint.pointPercentage];

        if (currentPoint.streetIndex == point2.streetIndex && currentPoint.pointPercentage == point2.pointPercentage) {
            break;  
        }

        if (currentStreetIndex == point2.streetIndex) {
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(worldCoordinates[point2.streetIndex].points[point2.pointPercentage]);
            currentPoint = point2; 
            break;
        }

        int nextStreetIndex = (i + 1 < streetPath.size()) ? streetPath[i + 1] : -1;
        glm::vec3 closestPointOnNextStreet = findClosestPointInStreet(nextStreetIndex, currentPointWorld);

        glm::vec3 closestPointOnCurrentStreet = findClosestPointInStreet(currentStreetIndex, closestPointOnNextStreet);

        float distanceToCurrentPoint = glm::length(closestPointOnCurrentStreet - currentPointWorld);
        if (distanceToCurrentPoint > 0.0f) {
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(closestPointOnCurrentStreet);
            currentPoint = updateCurrentPoint(currentStreetIndex, closestPointOnCurrentStreet);
            currentPointWorld = closestPointOnCurrentStreet; 
        }

        float distanceToNextStreetFromCurrent = glm::length(currentPointWorld - closestPointOnNextStreet);
        if (distanceToNextStreetFromCurrent > 0.0f) {
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(closestPointOnNextStreet);
            currentPoint = updateCurrentPoint(nextStreetIndex, closestPointOnNextStreet);
        }
    }
}
