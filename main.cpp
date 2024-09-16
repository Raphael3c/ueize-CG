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
        drawStreets(worldCoordinates, closestStreetIndex, closestPointPercentage);        // Chamada para desenhar as ruas
        drawInfiniteLines();  // Chamada para desenhar as linhas de referência
        printWorldCoordinates(); // Imprime coordenadas de ruas apenas uma vez

        // Trocar os buffers e processar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Processar entrada do teclado
        processInput(window);
    }

    glfwTerminate();
    return 0;
}
