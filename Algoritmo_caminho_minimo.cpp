#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Definição da estrutura do nó do grafo
struct Node {
    float id; // ID do nó (agora do tipo float)
    vector<pair<float, float>> neighbors; // Lista de vizinhos (id, custo para o vizinho)
    Node(float _id) : id(_id) {} // Construtor para criar um nó com ID
    Node() : id(-1.0f) {} // Construtor padrão para nós temporários
};

// Definição do grafo
struct Graph {
    unordered_map<float, Node> nodes; // Mapeamento de ID para nó
    void addNode(float id) {
        if (nodes.find(id) == nodes.end()) { // Adiciona um nó se ainda não existir
            nodes[id] = Node(id);
        }
    }
    void addEdge(float from, float to, float cost) {
        addNode(from); // Adiciona os nós de origem e destino
        addNode(to);
        nodes[from].neighbors.push_back({to, cost}); // Adiciona a aresta de from para to
        nodes[to].neighbors.push_back({from, cost}); // Considerando uma aresta bidirecional
    }
};

// Estrutura para representar um ponto (usado na heurística)
struct Point {
    float id; // ID do ponto (agora do tipo float)
    Point(float _id) : id(_id) {}
};

// Função heurística (distância entre dois pontos)
float heuristic(const Point& a, const Point& b) {
    return abs(a.id - b.id);
}

// Função para reconstruir o caminho a partir do mapa de nós
vector<float> reconstructPath(unordered_map<float, float>& cameFrom, float current) {
    vector<float> path;
    while (cameFrom.find(current) != cameFrom.end()) {
        path.push_back(current);
        current = cameFrom[current];
    }
    path.push_back(current);
    reverse(path.begin(), path.end());
    return path;
}

// Função para encontrar o caminho usando o algoritmo A*
vector<float> findPath(Graph& graph, const Point& start, const Point& end) {
    unordered_map<float, float> cameFrom; // Mapeamento de nós para seus predecessores
    unordered_map<float, float> costSoFar; // Custo acumulado para cada nó
    priority_queue<pair<float, float>, vector<pair<float, float>>, greater<pair<float, float>>> pq; // Fila de prioridade (custo, nó)
    pq.push({0.0f, start.id}); // Adiciona o nó inicial com custo zero
    costSoFar[start.id] = 0.0f; // Define o custo zero para o nó inicial
    while (!pq.empty()) {
        float currentCost = pq.top().first; // Custo atual
        float currentId = pq.top().second; // ID do nó atual
        pq.pop(); // Remove o nó da fila de prioridade
        if (currentId == end.id) { // Se chegou ao destino
            return reconstructPath(cameFrom, end.id); // Reconstrói o caminho
        }
        for (auto& neighbor : graph.nodes[currentId].neighbors) { // Para cada vizinho do nó atual
            float neighborId = neighbor.first; // ID do vizinho
            float cost = neighbor.second; // Custo para o vizinho
            float newCost = costSoFar[currentId] + cost; // Novo custo acumulado até o vizinho
            if (costSoFar.find(neighborId) == costSoFar.end() || newCost < costSoFar[neighborId]) {
                costSoFar[neighborId] = newCost; // Atualiza o custo acumulado
                float priority = newCost + heuristic(Point(neighborId), end); // Prioridade para o vizinho
                pq.push({priority, neighborId}); // Adiciona o vizinho na fila de prioridade
                cameFrom[neighborId] = currentId; // Atualiza o predecessor do vizinho
            }
        }
    }
    return vector<float>(); // Se não encontrar caminho, retorna um vetor vazio
}

int main() {
    // Criando o grafo com IDs de nós e custos das arestas em ponto flutuante
    Graph graph;
    graph.addEdge(0.0f, 1.0f, 2.0f);
    graph.addEdge(0.2f, 0.0f, 0.0f);
    graph.addEdge(0.2f, 1.0f, 0.0f);
    graph.addEdge(0.0f, 2.0f, 2.0f);
    graph.addEdge(0.0f, 3.0f, 4.0f);
    graph.addEdge(0.0f, 5.0f, 6.0f);
    graph.addEdge(0.0f, 4.0f, 7.0f);
    graph.addEdge(1.0f, 4.0f, 1.0f);
    graph.addEdge(2.0f, 4.0f, 3.0f);
    graph.addEdge(3.0f, 5.0f, 1.0f);
    graph.addEdge(5.0f, 4.0f, 5.0f);

    // Definindo o ponto de partida e destino
    Point start(0.2f);
    Point end(4.0f);

    // Encontrando o caminho no grafo
    vector<float> path = findPath(graph, start, end);

    // Exibindo o resultado
    if (!path.empty()) {
        cout << "Caminho encontrado:" << endl;
        for (float id : path) {
            cout << id << " ";
        }
        cout << endl;
    } else {
        cout << "Não foi possível encontrar um caminho." << endl;
    }

    return 0;
}
