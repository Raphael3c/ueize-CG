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
#include <queue>
#include <iomanip>

// Módulos personalizados
#include "drawsModule/drawInfiniteLines/main.h"
#include "drawsModule/drawStreets/main.h"
#include "drawsModule/drawLineFollowingStreetTopology/main.h"
#include "drawsModule/drawLine/main.h"
#include "utils/findPathBetweenStreets/main.h"
#include "utils/screenToWorldRay/main.h"

// Armazenamento das coordenadas das ruas e conexões
std::unordered_map<int, StreetPoints> worldCoordinates;
std::unordered_map<int, std::vector<int>> streetConnections;

// Variáveis globais da câmera
float cameraX = 6.12207127f, cameraY = 289.141296f, cameraZ = 40.0386086f;
float cameraYaw = glm::degrees(atan2(-cameraZ, -cameraX));
float cameraPitch = glm::degrees(atan2(-cameraY, sqrt(cameraX * cameraX + cameraZ * cameraZ)));
float cameraSpeed = 2.0f, mouseSensitivity = 0.1f;
float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true, controlCamera = true;

int currentCameraIndex = 0;
bool isCameraMoving = false;
std::vector<glm::vec3> cameraPath;  // Para armazenar o caminho da câmera
float cameraMoveSpeed = 0.5f; // Velocidade do movimento

// Variáveis de backup da câmera
float cameraXBP = 6.12207127f, cameraYBP = 289.141296f, cameraZBP = 40.0386086f;

// Variáveis de controle de iluminação e tempo
bool lanternEnabled = true;  // Lanterna ligada por padrão
bool sunEnabled = true;      // Sol ligado por padrão
bool sunRotationEnabled = false;  // Controle para rodar o sol
float sunRotationAngle = 0.0f;   // Ângulo de rotação do sol
// Velocidade de rotação do sol para 1 ciclo de 360 graus em 4 segundos
const float SUN_ROTATION_SPEED = 45.0f;  // 90 graus por segundo

// Variáveis de cursor e clique
double mouseX = 0.0, mouseY = 0.0;
int closestStreetIndex = -1;
float closestPointPercentage = -1.0f;
bool mousePressed = false;

// Vetores para armazenar pontos selecionados e coordenadas da linha
std::vector<SelectedPoint> selectedPoints(2, {0, 0.0f});
std::vector<glm::vec3> lineCoordinates;
int pointCount = 0;

// Inicialização das conexões entre ruas
void initializeStreetConnections() {
    streetConnections = {
        {0, {2, 3, 4}}, {1, {2, 3, 4, 6}}, {2, {0, 1}}, {3, {0, 1}},
        {4, {0, 1, 5}}, {5, {4, 6}}, {6, {1, 5}}
    };
}

// Variáveis da luz do sol
Light sun = {
    glm::vec3(0.8f),   // ambient - luz ambiente forte e amarelada
    glm::vec3(0.9f, 0.9f, 0.8f),   // diffuse - luz difusa muito intensa
    glm::vec3(1.0f, 1.0f, 0.8f),   // specular - brilho especular alto, quase branco
    glm::normalize(-glm::vec3(-0.2f, -1.0f, -0.3f)), // direção da luz
    glm::vec3(0.0f, 90.0f, 0.0f)    // posição (geralmente irrelevante para luzes direcionais)
};

// Função para atualizar a posição da câmera ao longo do caminho
void updateCameraPosition() {
    if (isCameraMoving && currentCameraIndex < cameraPath.size()) {
        glm::vec3 targetPoint = cameraPath[currentCameraIndex];

        // Calcula a direção e o passo da câmera
        glm::vec3 direction = targetPoint - glm::vec3(cameraX, cameraY, cameraZ);

        // Verifica se o vetor direction tem tamanho não-nulo
        if (glm::length(direction) > 0.001f) {
            direction = glm::normalize(direction);  // Somente normaliza se for válido
            glm::vec3 step = direction * cameraMoveSpeed;

            // Atualiza a posição da câmera
            cameraX += step.x;
            cameraY += step.y;  // Adiciona a altura extra
            cameraZ += step.z;
        }

        // Verifica se a câmera chegou ao ponto atual
        if (glm::length(glm::vec3(cameraX, cameraY, cameraZ) - targetPoint) < 0.5f) {
            currentCameraIndex++;
        }

        // Se chegou ao final do caminho
        if (currentCameraIndex >= cameraPath.size()) {
            isCameraMoving = false;

            // Reiniciar a câmera para as posições de backup
            cameraX = cameraXBP;
            cameraY = cameraYBP;  // Reinicia com a altura extra
            cameraZ = cameraZBP;
        }
    }
}

// Função para processar o movimento do sol
void updateSunPosition(float deltaTime) {
    if (sunRotationEnabled) {
        // Atualiza o ângulo do sol com base no tempo delta e velocidade de rotação
        sunRotationAngle += SUN_ROTATION_SPEED * deltaTime;  // Aumenta o ângulo rapidamente
        if (sunRotationAngle > 360.0f) {
            sunRotationAngle -= 360.0f;  // Mantém o ângulo no intervalo 0-360
        }
        // O sol vai girar em torno de um círculo em torno do observador (eixo Y)
        sun.direction = glm::normalize(glm::vec3(cos(glm::radians(sunRotationAngle)), sin(glm::radians(sunRotationAngle)), -0.3f));
    }
}

// Função para processar o clique do mouse
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 600.0f);
        glm::mat4 viewMatrix = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), 
            glm::vec3(cameraX + cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
            cameraY + sin(glm::radians(cameraPitch)),
            cameraZ + sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch))), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 ray = screenToWorldRay((int)mouseX, (int)mouseY, windowWidth, windowHeight, projectionMatrix, viewMatrix);
        glm::vec3 cameraPosition = glm::vec3(cameraX, cameraY, cameraZ);
        float minDistance = FLT_MAX;

        // Encontrar o ponto mais próximo
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
            // Armazena os pontos selecionados
            selectedPoints[pointCount % 2] = {closestStreetIndex, closestPointPercentage};
            pointCount++;

            // Se dois pontos forem selecionados, desenha a linha e move a câmera
            if (pointCount >= 2) {
                lineCoordinates.clear();  // Limpa antes de traçar nova linha
                drawLineFollowingStreetTopology(selectedPoints[0], selectedPoints[1], streetConnections);

                // Atualizar caminho da câmera
                cameraPath = lineCoordinates;  // Atribuir o caminho para a câmera
                currentCameraIndex = 0;  // Resetar o índice da câmera
                isCameraMoving = true;  // Ativar o movimento

                // Teletransportar a câmera para o início do caminho
                if (!cameraPath.empty()) {
                    cameraX = cameraPath[0].x;
                    cameraY = cameraPath[0].y + 20.0f;  // Um pouco acima do ponto inicial
                    cameraZ = cameraPath[0].z;
                }
            }
        }
    }
}

// Função para processar o movimento do mouse
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;

    if (!controlCamera) return;

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

    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;
}

// Função para processar entrada de teclado
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        controlCamera = !controlCamera;
        glfwSetInputMode(window, GLFW_CURSOR, controlCamera ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    glm::vec3 cameraFront = glm::normalize(glm::vec3(
        cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
        sin(glm::radians(cameraPitch)),
        sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch))));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraX += cameraSpeed * cameraFront.x;
        cameraY += cameraSpeed * cameraFront.y;
        cameraZ += cameraSpeed * cameraFront.z;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraX -= cameraSpeed * cameraFront.x;
        cameraY -= cameraSpeed * cameraFront.y;
        cameraZ -= cameraSpeed * cameraFront.z;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraX -= glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).x * cameraSpeed;
        cameraZ -= glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).z * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraX += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).x * cameraSpeed;
        cameraZ += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).z * cameraSpeed;
    }

    // Alternar a lanterna
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        lanternEnabled = !lanternEnabled;
    }

    // Alternar o sol
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        sunEnabled = !sunEnabled;
    }

    // Alternar rotação do sol
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        sun.position.y = -sun.position.y;
    }
}

// Inicialização de renderização suave
void initSmoothRendering() {
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

int main(void) {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "3D Picking with Mouse", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    initSmoothRendering();
    initializeStreetConnections();

    float lastFrameTime = glfwGetTime();  // Armazenar o tempo do último frame

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        float aspectRatio = (float)windowWidth / (float)windowHeight;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspectRatio, 0.1, 1000.0);  // Aumente o far clipping plane de 600.0 para 1000.0

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Calcula o delta time (tempo entre o frame atual e o último)
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Atualizar a posição da câmera ao longo do caminho
        updateCameraPosition();

        // Atualizar a posição do sol com base no delta time
        updateSunPosition(deltaTime);

        glm::vec3 cameraFront(
            cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
            sin(glm::radians(cameraPitch)),
            sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)));

        glm::vec3 cameraPos(cameraX, cameraY, cameraZ);
        glm::vec3 cameraTarget = cameraPos + cameraFront;

        gluLookAt(cameraX, cameraY + 10.0f, cameraZ, cameraTarget.x, cameraTarget.y + 10.0f, cameraTarget.z, 0.0f, 1.0f, 0.0f);

        Camera camera = { glm::vec3(cameraX, cameraY, cameraZ) };

        glm::vec3 offset = glm::vec3(0.0f, 8.0f, 0.0f); 
        glm::vec3 behind = -glm::normalize(cameraTarget - camera.position) * 15.0f;

        Light lantern = {
                glm::vec3(0.0f),  
                glm::vec3(0.0f),     
                glm::vec3(0.0f),   
                glm::normalize(camera.position),     
                camera.position + offset + behind
            };

        // Controla se a lanterna está ativa
        if (lanternEnabled) {
            lantern = {
                glm::vec3(0.05f, 0.05f, 0.05f),  
                glm::vec3(0.8f, 0.8f, 0.6f),     
                glm::vec3(0.8f, 0.8f, 0.8f),   
                glm::normalize(cameraTarget - camera.position),     
                camera.position + offset + behind
            };

            sun = {
                glm::vec3(0.0f),    
                glm::vec3(0.0f),    
                glm::vec3(0.0f),    
                glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)), 
                glm::vec3(0.0f, 0.0f, 0.0f)
            };

            glPointSize(10.0f);  // Aumenta o tamanho do ponto para que ele fique visível

            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f);  // Define a cor do ponto como vermelho (pode ser outra)
            glVertex3f(lantern.position.x, lantern.position.y, lantern.position.z);  // Posição da lanterna
            glEnd();
        }else {
            sun = {
                glm::vec3(0.01f, 0.01f, 0.01f),    
                glm::vec3(0.5f, 0.5f, 0.5f),    
                glm::vec3(0.6f, 0.6f, 0.6f),    
                glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)), 
                glm::vec3(0.0f, 0.0f, 0.0f)
            };
        }

        drawStreets(worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, camera, sun, lantern);

        drawInfiniteLines();
        drawLine();

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
    }

    glfwTerminate();
    return 0;
}
