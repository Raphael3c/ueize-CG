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
float cameraX = 6.12207127f, cameraY = 289.141296f, cameraZ = 40.0386086f;

float cameraXS = 6.12207127f, cameraYS = 289.141296f, cameraZS = 40.0386086f;

float cameraYaw = glm::degrees(atan2(-cameraZ, -cameraX));  // Calcular yaw baseado na posição atual
float cameraPitch = glm::degrees(atan2(-cameraY, sqrt(cameraX * cameraX + cameraZ * cameraZ)));  // Calcular pitch
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

//Definindo a posição da luz
Light light = {
    glm::vec3(0.0f, -1.0f, 0.0f),  // direção da luz
    glm::vec3(0.9f, 0.9f, 0.9f),  // ambient - quase branco/cinza
    glm::vec3(0.8f, 0.8f, 0.8f),  // diffuse - quase branco/cinza
    glm::vec3(1.0f, 1.0f, 1.0f),  // specular - brilho intenso
    glm::vec3(1.0f, 1.0f, 1.0f)   // posicao da luz
};

Light light_spot = {
    glm::vec3(0.0f, 0.0f, -1.0f),  // direção inicial (será atualizada para seguir a câmera)
    glm::vec3(0.1f, 0.1f, 0.1f),  // ambient - luz ambiente fraca
    glm::vec3(1.0f, 1.0f, 1.0f),  // diffuse - luz branca forte para simular o farol
    glm::vec3(1.0f, 1.0f, 1.0f),  // specular - brilho intenso
    glm::vec3(0.0f, 60.0f, 0.0f)  // posição inicial da luz (seguirá a câmera)
};

bool isCameraMoving = false;  // Controla se a câmera está em movimento
std::vector<glm::vec3> cameraPath;  // Armazena o caminho da câmera
int currentCameraIndex = 0;  // Índice do ponto atual no caminho da câmera
float cameraMoveSpeed = 0.2f;  // Velocidade de movimento da câmera
glm::vec3 cameraDirection;  // Direção atual da câmera

// Inicialização das conexões entre ruas
void initializeStreetConnections() {
    streetConnections = {
        {0, {2, 3, 4}}, {1, {2, 3, 4, 6}}, {2, {0, 1}}, {3, {0, 1}},
        {4, {0, 1, 5}}, {5, {4, 6}}, {6, {1, 5}}
    };
}

void drawLightSpotPosition(const glm::vec3& position) {
    glPointSize(30.0f);  // Defina o tamanho do ponto
    glColor3f(1.0f, 1.0f, 1.0f);  // Cor amarela para o ponto, para destacar a luz

    glBegin(GL_POINTS);
        glVertex3f(position.x, position.y, position.z);  // Desenha o ponto na posição da luz spot
    glEnd();
}


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

                // Iniciar o movimento da câmera ao longo do caminho
                cameraPath = lineCoordinates;  // Atribui as coordenadas da linha ao caminho da câmera
                isCameraMoving = true;  // Iniciar o movimento
                controlCamera = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                currentCameraIndex = 0;  // Reseta o índice de controle do caminho

                // Reposicionar a câmera ligeiramente acima do ponto inicial
                glm::vec3 startPosition = cameraPath[0];
                cameraX = startPosition.x;
                cameraY = startPosition.y + 20.0f;  // A câmera está 10 unidades acima do ponto inicial
                cameraZ = startPosition.z;

                // Definir a direção inicial da câmera para o próximo ponto
                cameraDirection = glm::normalize(cameraPath[1] - startPosition);
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

void processInput(GLFWwindow* window) {
    if(isCameraMoving) return;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        controlCamera = !controlCamera;
        glfwSetInputMode(window, GLFW_CURSOR, controlCamera ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    // Calcula o vetor para onde a câmera está "frente"
    glm::vec3 cameraFront = glm::normalize(glm::vec3(
        cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
        sin(glm::radians(cameraPitch)),
        sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch))
    ));

    // Calcula o vetor "direita" usando o produto vetorial entre a frente e o vetor "up" (que no caso é (0,1,0) para manter o eixo Y como 'cima')
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));

    // O vetor "up" da câmera permanece fixo no eixo Y, já que não estamos girando no plano vertical
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Movimento para frente (W) e para trás (S)
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

    // Movimento para esquerda (A) e direita (D)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraX -= cameraSpeed * cameraRight.x;
        cameraZ -= cameraSpeed * cameraRight.z;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraX += cameraSpeed * cameraRight.x;
        cameraZ += cameraSpeed * cameraRight.z;
    }
}

// Inicialização de renderização suave
void initSmoothRendering() {
    glEnable(GL_POINT_SMOOTH);
    // glEnable(GL_LINE_SMOOTH);
    // glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

float timeOfDay = 0.0f;  // Começa à meia-noite

void updateLightProperties(Light& light, float timeOfDay) {
    float angle = glm::radians(timeOfDay);
    
    // Simular a direção do Sol
    light.direction = glm::normalize(glm::vec3(sin(angle), cos(angle), 0.0f));
    
    // Alterar a cor da luz ao longo do dia (mais quente pela manhã e fria à tarde)
    if (timeOfDay < 180.0f) {
        light.diffuse = glm::vec3(1.0f, 0.9f, 0.7f);  // Manhã (amarelo/laranja)
    } else {
        light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);  // Tarde (branco)
    }
    
    // À noite, apagar a luz
    if (timeOfDay > 240.0f && timeOfDay < 360.0f) {
        light.ambient = glm::vec3(0.1f, 0.1f, 0.2f);  // Luz ambiente fraca
        light.diffuse = glm::vec3(0.0f, 0.0f, 0.1f);  // Luz difusa quase apagada
        light.specular = glm::vec3(0.0f);  // Sem brilho especular
    }
}

// Função para desenhar o ponto de luz orbitando em torno do eixo Z em direção ao eixo X
void drawLightOrbit(const Light& light, float timeOfDay) {
    float radius = 50.0f;  // Raio da órbita do Sol
    float angle = glm::radians(timeOfDay);  // Converte o tempo do dia em ângulo

    // Calcular a posição da luz (ponto orbitando ao redor do eixo Z)
    float x = radius * cos(angle);  // Rotação em torno do eixo Z
    float y = radius * sin(angle);  // Rotação em torno do eixo Z
    glm::vec3 lightPosition = glm::vec3(x, y, 80.0f);  // Mantendo Z fixo para simular o movimento ao longo de X-Y

    // Desenhar o ponto de luz na órbita
    glPointSize(15.0f);  // Tamanho do ponto para ficar mais visível
    glColor3f(1.0f, 1.0f, 0.0f);  // Cor amarela para simular o Sol
    glBegin(GL_POINTS);
        glVertex3f(lightPosition.x, lightPosition.y, lightPosition.z);  // Posição do ponto de luz
    glEnd();
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

    if (isCameraMoving && !cameraPath.empty()) {
        glm::vec3 targetPoint = cameraPath[currentCameraIndex];

        // Calcula a direção e a movimentação da câmera
        glm::vec3 moveDirection = glm::normalize(targetPoint - glm::vec3(cameraX, cameraY, cameraZ));
        glm::vec3 moveStep = moveDirection * cameraMoveSpeed;

        // Atualiza a posição da câmera
        cameraX += moveStep.x;
        cameraY += moveStep.y;
        cameraZ += moveStep.z;

        // Verifica se a câmera chegou ao ponto alvo e atualiza para o próximo ponto
        if (glm::length(glm::vec3(cameraX, cameraY, cameraZ) - targetPoint) < 0.1f) {
            currentCameraIndex++;
            if (currentCameraIndex >= cameraPath.size()) {
                isCameraMoving = false;  // Movimento concluído
                cameraX = cameraXS;
                cameraY = cameraYS;
                cameraZ = cameraZS;

                cameraYaw = glm::degrees(atan2(-cameraZ, -cameraX));  // Calcular yaw baseado na posição atual
                cameraPitch = glm::degrees(atan2(-cameraY, sqrt(cameraX * cameraX + cameraZ * cameraZ)));  // Calcular pitch
            }
        }

        // Atualizar a direção da câmera para seguir o próximo ponto
        if (currentCameraIndex < cameraPath.size() - 1) {
            cameraDirection = glm::normalize(cameraPath[currentCameraIndex + 1] - glm::vec3(cameraX, cameraY, cameraZ));
        }
    }

    // Restante do código de movimentação da câmera
    glm::vec3 cameraFront(
        cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
        sin(glm::radians(cameraPitch)),
        sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)));

    glm::vec3 cameraPos(cameraX, cameraY, cameraZ);
    glm::vec3 cameraTarget = cameraPos + cameraFront;

    gluLookAt(cameraX, cameraY, cameraZ, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0.0f, 1.0f, 0.0f);
    
    // Atualiza a posição da luz spot para a posição da câmera
    light_spot.position = glm::vec3(cameraX, cameraY, cameraZ);

    // Atualiza a direção da luz para seguir a direção que a câmera está "olhando"
    light_spot.direction = glm::normalize(cameraFront);
    
    Camera camera = {
        glm::vec3(cameraX, cameraY, cameraZ),  
    };

    // drawLightOrbit(light, timeOfDay);
    updateLightProperties(light, timeOfDay);

    // Incrementar o tempo do dia (controla a velocidade do movimento do Sol)
    timeOfDay += 0.5f;

    if (timeOfDay > 360.0f) {
        timeOfDay = 0.0f;  // Reseta o ciclo para simular o próximo dia
    }

    drawStreets(
        worldCoordinates, 
        closestStreetIndex, 
        closestPointPercentage, 
        selectedPoints, camera, 
        light, 
        light_spot
    );
    
    
    drawInfiniteLines();
    drawLine();

    glfwSwapBuffers(window);
    glfwPollEvents();

    processInput(window);
}

    glfwTerminate();
    return 0;
}
