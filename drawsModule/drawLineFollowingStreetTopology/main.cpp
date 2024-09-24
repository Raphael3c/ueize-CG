#include "main.h"
#include "../../common.h"

// Declaração global de worldCoordinates e lineCoordinates
extern std::unordered_map<int, StreetPoints> worldCoordinates;
extern std::vector<glm::vec3> lineCoordinates;
extern std::vector<int> findPathBetweenStreets(int startStreet, int endStreet, const std::unordered_map<int, std::vector<int>>& streetConnections);

// Função auxiliar para encontrar o ponto mais próximo em uma rua
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

// Função auxiliar para atualizar o ponto atual
SelectedPoint updateCurrentPoint(int streetIndex, const glm::vec3& newPoint) {
    auto it = std::find_if(
        worldCoordinates[streetIndex].points.begin(),
        worldCoordinates[streetIndex].points.end(),
        [&](const std::pair<float, glm::vec3>& p) { return p.second == newPoint; }
    );
    return {streetIndex, it->first};
}

// Função principal para traçar linha seguindo a topologia das ruas
void drawLineFollowingStreetTopology(const SelectedPoint& point1, const SelectedPoint& point2, std::unordered_map<int, std::vector<int>> streetConnections) {
    lineCoordinates.clear();  // Limpar as coordenadas da linha anterior

    // 1. Obter o caminho entre as ruas usando o findPathBetweenStreets
    std::vector<int> streetPath = findPathBetweenStreets(point1.streetIndex, point2.streetIndex, streetConnections);

    // 2. Definir o ponto inicial como o ponto1
    SelectedPoint currentPoint = point1;

    // 3. Loop para seguir o caminho de ruas até o ponto destino
    for (size_t i = 0; i < streetPath.size(); ++i) {
        int currentStreetIndex = streetPath[i];
        glm::vec3 currentPointWorld = worldCoordinates[currentStreetIndex].points[currentPoint.pointPercentage];

        // 3.1 Verificar se o ponto atual é o ponto destino
        if (currentPoint.streetIndex == point2.streetIndex && currentPoint.pointPercentage == point2.pointPercentage) {
            break;  // Se estamos no destino, terminamos
        }

        // 4. Verificar se estamos na rua do ponto destino
        if (currentStreetIndex == point2.streetIndex) {
            // 4.1 Traçar linha até o ponto destino e encerrar
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(worldCoordinates[point2.streetIndex].points[point2.pointPercentage]);
            currentPoint = point2;  // Atualizamos o ponto atual para o destino
            break;
        }

        // 5. Encontrar o ponto mais próximo da próxima rua
        int nextStreetIndex = (i + 1 < streetPath.size()) ? streetPath[i + 1] : -1;
        glm::vec3 closestPointOnNextStreet = findClosestPointInStreet(nextStreetIndex, currentPointWorld);

        // 5.1 Encontrar o ponto mais próximo da rua atual que é o mais próximo da próxima rua
        glm::vec3 closestPointOnCurrentStreet = findClosestPointInStreet(currentStreetIndex, closestPointOnNextStreet);

        // 5.2 Verificar se o ponto P1 (mais próximo na rua atual) é diferente do ponto atual
        float distanceToCurrentPoint = glm::length(closestPointOnCurrentStreet - currentPointWorld);
        if (distanceToCurrentPoint > 0.0f) {
            // Traçar linha entre o ponto atual e o ponto P1
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(closestPointOnCurrentStreet);
            currentPoint = updateCurrentPoint(currentStreetIndex, closestPointOnCurrentStreet);
            currentPointWorld = closestPointOnCurrentStreet;  // Atualizar o ponto atual
        }

        // 5.3 Traçar linha entre o ponto atual (P1) e o ponto mais próximo da próxima rua (P2)
        float distanceToNextStreetFromCurrent = glm::length(currentPointWorld - closestPointOnNextStreet);
        if (distanceToNextStreetFromCurrent > 0.0f) {
            lineCoordinates.push_back(currentPointWorld);
            lineCoordinates.push_back(closestPointOnNextStreet);
            currentPoint = updateCurrentPoint(nextStreetIndex, closestPointOnNextStreet);  // Atualizar o ponto atual para P2
        }
    }
}
