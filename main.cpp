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

// Meus módulos
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
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;

float cameraYaw = -90.0f, cameraPitch = 0.0f;
float cameraSpeed = 0.5f, mouseSensitivity = 0.1f;

float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true, controlCamera = true, printOnlyOne = true;

// Variáveis de cursor e clique
float customCursorX = 0.0f, customCursorY = 0.0f;
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

// Função para processar o clique do mouse
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;

        // Configurações de câmera e ray casting
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

            if (pointCount >= 2) {
                drawLineFollowingStreetTopology(selectedPoints[0], selectedPoints[1], streetConnections);
            }
        }
    }
}

// Função para processar o movimento do mouse
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraX += cameraSpeed * cameraFront.x, cameraY += cameraSpeed * cameraFront.y, cameraZ += cameraSpeed * cameraFront.z;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraX -= cameraSpeed * cameraFront.x, cameraY -= cameraSpeed * cameraFront.y, cameraZ -= cameraSpeed * cameraFront.z;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraX -= glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).x * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraX += glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))).x * cameraSpeed;
}

// Inicialização de renderização suave
void initSmoothRendering() {
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
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

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        float aspectRatio = (float)windowWidth / (float)windowHeight;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspectRatio, 0.1, 600.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glm::vec3 cameraFront(
            cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
            sin(glm::radians(cameraPitch)),
            sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)));

        glm::vec3 cameraPos(cameraX, cameraY, cameraZ);
        glm::vec3 cameraTarget = cameraPos + cameraFront;
        gluLookAt(cameraX, cameraY, cameraZ, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0.0f, 1.0f, 0.0f);

        Camera camera = {
            glm::vec3(cameraX, cameraY, cameraZ),  
        };

        drawStreets(worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, camera);
        drawInfiniteLines();
        drawLine();

        glfwSwapBuffers(window);
        glfwPollEvents();

        processInput(window);
    }

    glfwTerminate();
    return 0;
}
