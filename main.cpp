#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>          

#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip> 

//Meus módulos
#include "drawsModule/drawInfiniteLines/main.h"
#include "drawsModule/drawStreets/main.h"

#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm> 

// Armazenamento das coordenadas das ruas
std::unordered_map<int, StreetPoints> worldCoordinates;

// Variáveis globais para a posição da câmera
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;
float cameraYaw = -90.0f, cameraPitch = 0.0f;
float cameraSpeed = 0.5f;
float mouseSensitivity = 0.1f;
float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true;
bool controlCamera = true;  // Flag para alternar entre controlar a câmera ou o cursor
bool printOnlyOne = true;  

// Variáveis para o cursor personalizado
float customCursorX = 0.0f, customCursorY = 0.0f;

// Variáveis de posição do mouse e para o clique
int closestStreetIndex = -1;  // Rua mais próxima do clique
float closestPointPercentage = -1;  // Ponto mais próximo do clique
double mouseX = 0.0;
double mouseY = 0.0;
bool mousePressed = false;

// Vetor para armazenar os dois últimos pontos
std::vector<SelectedPoint> selectedPoints(2, {0, 0.0f});
int pointCount = 0;  // Contador de pontos para alternar entre os dois slots

// Armazenar as coordenadas da linha traçada
std::vector<glm::vec3> lineCoordinates;

// Função que converte coordenadas de tela para um raio no mundo
glm::vec3 screenToWorldRay(int mouseX, int mouseY, int windowWidth, int windowHeight, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // Coordenadas normalizadas entre -1 e 1
    float x = (2.0f * mouseX) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / windowHeight; // Coordenadas Y invertidas
    float z = 1.0f; // Far plane

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
    
    // Transformar de clip-space para eye-space
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);  // Definir z=-1, w=0 para o vetor direcional

    // Transformar de eye-space para world-space
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    
    return rayWorld;
}

// Estrutura para armazenar as conexões entre ruas (um grafo simples)
std::unordered_map<int, std::vector<int>> streetConnections;

void initializeStreetConnections() {
    streetConnections[0] = {2,3,4};  // Rua 0 está conectada à rua 1
    streetConnections[1] = {2,3,4,6};  // Rua 1 está conectada à rua 0 e 2
    streetConnections[2] = {0,1};  // Rua 2 está conectada à rua 1
    streetConnections[3] = {0,1};  // Rua 2 está conectada à rua 1
    streetConnections[4] = {0,1,5};  // Rua 2 está conectada à rua 1
    streetConnections[5] = {4,6};  // Rua 2 está conectada à rua 1
    streetConnections[6] = {1,6};  // Rua 2 está conectada à rua 1
}

// Função para encontrar o caminho entre ruas usando BFS
std::vector<int> findPathBetweenStreets(int startStreet, int endStreet) {
    std::unordered_map<int, int> previous;  // Para armazenar o caminho
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
        for (int neighbor : streetConnections[currentStreet]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push(neighbor);
                previous[neighbor] = currentStreet;
            }
        }
    }

    return {};  // Retornar caminho vazio se não houver caminho
}

void drawLineFollowingStreetTopology(const SelectedPoint& point1, const SelectedPoint& point2) {
    lineCoordinates.clear();  // Limpar as coordenadas da linha anterior

    // 1. Obter o caminho entre as ruas usando o findPathBetweenStreets
    std::vector<int> streetPath = findPathBetweenStreets(point1.streetIndex, point2.streetIndex);

    // 2. Definir o ponto inicial como o ponto1
    SelectedPoint currentPoint = point1;

    // Função auxiliar para encontrar o ponto mais próximo em uma rua
    auto findClosestPointInStreet = [&](int streetIndex, const glm::vec3& targetPoint) -> glm::vec3 {
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
    };

    // Função auxiliar para atualizar o ponto atual
    auto updateCurrentPoint = [&](int streetIndex, const glm::vec3& newPoint) -> SelectedPoint {
        auto it = std::find_if(
            worldCoordinates[streetIndex].points.begin(),
            worldCoordinates[streetIndex].points.end(),
            [&](const std::pair<float, glm::vec3>& p) { return p.second == newPoint; }
        );
        return {streetIndex, it->first};
    };

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

// #TODO Remover daqui
// Função para desenhar a linha
void drawLine() {
    if (lineCoordinates.empty()) return;  // Não há linha para desenhar

    glColor3f(1.0f, 0.0f, 1.0f);  // Definir a cor da linha para branco
    glBegin(GL_LINE_STRIP);
    for (const auto& point : lineCoordinates) {
        glVertex3f(point.x, point.y, point.z);  // Definir os vértices da linha
    }
    glEnd();
}


// Função de callback para o clique do mouse
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;

        // Configurar a câmera para ray casting
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 600.0f);
        glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(cameraX, cameraY, cameraZ), 
            glm::vec3(
                cameraX + cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
                cameraY + sin(glm::radians(cameraPitch)),
                cameraZ + sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch))
            ),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::vec3 ray = screenToWorldRay((int)mouseX, (int)mouseY, windowWidth, windowHeight, projectionMatrix, viewMatrix);

        glm::vec3 cameraPosition = glm::vec3(cameraX, cameraY, cameraZ);
        float minDistance = FLT_MAX;
        closestStreetIndex = -1;
        closestPointPercentage = -1;

        // Encontrar o ponto mais próximo da rua
        for (const auto& street : worldCoordinates) {
            for (const auto& point : street.second.points) {
                glm::vec3 toPoint = point.second - cameraPosition;
                float t = glm::dot(toPoint, ray);
                glm::vec3 closestPointOnRay = cameraPosition + ray * t;
                float distance = glm::length(closestPointOnRay - point.second);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestStreetIndex = street.first;
                    closestPointPercentage = point.first;
                }
            }
        }

        if (closestStreetIndex != -1) {
            std::cout << "Ponto mais próximo: Rua " << closestStreetIndex 
                      << ", Posição " << closestPointPercentage << "%\n";

            selectedPoints[pointCount % 2] = {closestStreetIndex, closestPointPercentage};
            pointCount++;

            // Se temos dois pontos selecionados, desenhar a linha
            if (pointCount >= 2) {
                drawLineFollowingStreetTopology(selectedPoints[0], selectedPoints[1]);  // Ruas diferentes
            }
        }
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;

    if (!controlCamera) {
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        customCursorX = (xpos / windowWidth) * 2.0f - 1.0f;
        customCursorY = 1.0f - (ypos / windowHeight) * 2.0f;
        return;
    }

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    cameraYaw += xOffset;
    cameraPitch += yOffset;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        controlCamera = !controlCamera;
        if (!controlCamera) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
        }
    }

    if (controlCamera) {
        glm::vec3 cameraFront;
        cameraFront.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront.y = sin(glm::radians(cameraPitch));
        cameraFront.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(cameraFront); 

        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))); 
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraX += cameraSpeed * cameraFront.x, cameraY += cameraSpeed * cameraFront.y, cameraZ += cameraSpeed * cameraFront.z;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraX -= cameraSpeed * cameraFront.x, cameraY -= cameraSpeed * cameraFront.y, cameraZ -= cameraSpeed * cameraFront.z;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraX -= cameraRight.x * cameraSpeed, cameraY -= cameraRight.y * cameraSpeed, cameraZ -= cameraRight.z * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraX += cameraRight.x * cameraSpeed, cameraY += cameraRight.y * cameraSpeed, cameraZ += cameraRight.z * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraY += cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cameraY -= cameraSpeed;
    }
}

void initSmoothRendering() {
    // Suavizar pontos
    glEnable(GL_POINT_SMOOTH);
    
    // Suavizar linhas
    glEnable(GL_LINE_SMOOTH);
    
    // Suavizar polígonos (opcional, pode melhorar a qualidade de objetos 3D)
    glEnable(GL_POLYGON_SMOOTH);
    
    // Habilitar blending para suavizar transições de cores nas bordas
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Configurar uma dica (hint) para indicar que queremos a melhor qualidade de suavização
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void printWorldCoordinates() {
    if(!printOnlyOne) return;
    
    for (const auto& street : worldCoordinates) {
        std::cout << "Street " << street.first << " coordinates:\n";
        for (const auto& point : street.second.points) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "  " << point.first << "%: (" 
                    << point.second.x << ", " 
                    << point.second.y << ", " 
                    << point.second.z << ")\n";
        }
    }

    printOnlyOne = false;
}

int main(void) {
    if (!glfwInit())
        return -1;

    // Desabilitar o modo fullscreen, configurando o monitor para NULL
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);  // Deixe a janela redimensionável, caso necessário

    GLFWwindow* window = glfwCreateWindow(1280, 720, "3D Picking with Mouse", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    // Ajustar o posicionamento da janela para garantir que ela não abra maximizada ou em modo tela cheia
    glfwSetWindowPos(window, 100, 100);  // Posiciona a janela no canto superior esquerdo (ajuste conforme desejar)


    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);  // Registrar o callback do clique do mouse

    // Inicializar suavização e outras configurações
    initSmoothRendering();

    initializeStreetConnections();

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Limpar buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Obter as dimensões da janela
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        float aspectRatio = (float)windowWidth / (float)windowHeight;

        // Definir a projeção em perspectiva ajustada ao tamanho da tela
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspectRatio, 0.1, 600.0);

        // Configurar a câmera (visão)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glm::vec3 cameraFront;
        cameraFront.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront.y = sin(glm::radians(cameraPitch));
        cameraFront.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

        glm::vec3 cameraPos(cameraX, cameraY, cameraZ);
        glm::vec3 cameraTarget = cameraPos + cameraFront;
        glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);  // Definindo o vetor "up"

        gluLookAt(cameraX, cameraY, cameraZ, cameraTarget.x, cameraTarget.y, cameraTarget.z, cameraUp.x, cameraUp.y, cameraUp.z);

        // Desenhar as ruas e as linhas
        drawStreets(worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints);        // Chamada para desenhar as ruas
        drawInfiniteLines();  // Chamada para desenhar as linhas de referência
        printWorldCoordinates(); // Imprime coordenadas de ruas apenas uma vez
        
        // #Todo: vai ser chamado de drawRoute
        drawLine();  // Desenhar a linha entre os pontos selecionados

        // Trocar os buffers e processar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Processar entrada do teclado
        processInput(window);
    }

    glfwTerminate();
    return 0;
}
