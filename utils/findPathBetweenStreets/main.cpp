#include "main.h"

// Função para encontrar o caminho entre ruas usando BFS
std::vector<int> findPathBetweenStreets(int startStreet, int endStreet, const std::unordered_map<int, std::vector<int>>& streetConnections) {
    std::unordered_map<int, int> previous;
    std::queue<int> queue;
    std::unordered_map<int, bool> visited;

    queue.push(startStreet);
    visited[startStreet] = true;

    while (!queue.empty()) {
        int currentStreet = queue.front();
        queue.pop();

        // Se encontramos a rua de destino
        if (currentStreet == endStreet) {
            std::vector<int> path;
            for (int at = endStreet; at != startStreet; at = previous[at]) {
                path.push_back(at);
            }
            path.push_back(startStreet);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Visitar ruas conectadas
        for (int neighbor : streetConnections.at(currentStreet)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push(neighbor);
                previous[neighbor] = currentStreet;
            }
        }
    }

    return {};  // Retornar caminho vazio se não houver caminho
}
