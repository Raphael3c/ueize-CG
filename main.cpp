#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Para funções de transformação de matriz como glm::perspective
#include <glm/gtc/type_ptr.hpp>          // Para usar glm::value_ptr se necessário

#include <GL/glu.h>
#include <GL/gl.h>


// Variáveis globais para a posição e orientação da câmera
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;  // Posição inicial da câmera
float cameraYaw = -90.0f, cameraPitch = 0.0f;           // Direção da câmera
float cameraSpeed = 0.5f;                              // Velocidade de movimento
float mouseSensitivity = 0.1f;
float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true;
bool cameraLocked = false;

// Variáveis de posição do mouse
double mouseX = 0.0;
double mouseY = 0.0;

// Função de callback para capturar o movimento do mouse
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Invertido porque as coordenadas Y da janela vão de cima para baixo

    lastX = xpos;
    lastY = ypos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    cameraYaw += xOffset;
    cameraPitch += yOffset;

    // Limitar o ângulo de pitch para não virar de cabeça para baixo
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

// Função para processar a entrada do teclado e mover a câmera
void processInput(GLFWwindow* window) {
    float cameraFrontX = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    float cameraFrontY = sin(glm::radians(cameraPitch));
    float cameraFrontZ = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraX += cameraSpeed * cameraFrontX, cameraY += cameraSpeed * cameraFrontY, cameraZ += cameraSpeed * cameraFrontZ;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraX -= cameraSpeed * cameraFrontX, cameraY -= cameraSpeed * cameraFrontY, cameraZ -= cameraSpeed * cameraFrontZ;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraX -= cameraSpeed * sin(glm::radians(cameraYaw)), cameraZ += cameraSpeed * cos(glm::radians(cameraYaw));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraX += cameraSpeed * sin(glm::radians(cameraYaw)), cameraZ -= cameraSpeed * cos(glm::radians(cameraYaw));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraY += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraY -= cameraSpeed;
}

// Função para desenhar as paredes e o chão
void drawStreets() {
    // Cor verde para as paredes (ruas)
    glColor3f(0.0f, 1.0f, 0.0f);

    // Variáveis para controlar a largura e o comprimento das ruas
    float streetLength = 200.0f;  // Comprimento das ruas
    float streetWidth = 30.0f;    // Largura da rua
    float streetHeight = 6.0f;    // Altura das ruas (equivalente às "paredes")
    float gapBetweenStreets = 5.0f; // Gap entre ruas paralelas

    // Largura total das ruas paralelas (incluindo o gap entre elas)
    float totalParallelWidth = (streetWidth * 3) + (gapBetweenStreets * 2);

    // Desenhar ruas paralelas
    for (int i = -1; i <= 3; i++) {  // 5 ruas no total: -1, 0, 1 (paralelas)
        float offsetX = i * (streetWidth + gapBetweenStreets);  // Offset entre as ruas
        float divideFactor = 2;

        float xPositionNegative = offsetX - streetWidth / 2;
        float xPositionPositive = offsetX + streetWidth / 2;

        if(i == 2){
            divideFactor = 3;
        }else if(i == 3){
            divideFactor = 4;
        }

        float zPositionNegative = -(streetLength / divideFactor);
        float zPositionPositive = (streetLength / 2);

        glBegin(GL_QUADS);
            // Desenhar cada rua (paralela)
            glVertex3f(xPositionNegative, 0.0f, zPositionNegative);
            glVertex3f(xPositionNegative, streetHeight, zPositionNegative);
            glVertex3f(xPositionNegative, streetHeight, zPositionPositive);
            glVertex3f(xPositionNegative, 0.0f, zPositionPositive);

            if(i == 1){
                glVertex3f(xPositionPositive, 0.0f, -(streetLength / 3));
                glVertex3f(xPositionPositive, streetHeight, -(streetLength / 3));
                glVertex3f(xPositionPositive, streetHeight, zPositionPositive);
                glVertex3f(xPositionPositive, 0.0f, zPositionPositive);
            }else {
                glVertex3f(xPositionPositive, 0.0f, zPositionNegative);
                glVertex3f(xPositionPositive, streetHeight, zPositionNegative);
                glVertex3f(xPositionPositive, streetHeight, zPositionPositive);
                glVertex3f(xPositionPositive, 0.0f, zPositionPositive);
            }
        glEnd();
    }

    // Cor cinza para o chão
    glColor3f(0.5f, 0.5f, 0.5f);
    for (int i = -1; i <= 3; i++) {
        float offsetX = i * (streetWidth + gapBetweenStreets);  // Offset entre as ruas
        float divideFactor = 2;

        if(i == 2){
            divideFactor = 3;
        }else if(i == 3){
            divideFactor = 4;
        }

        float zPositionNegative = -(streetLength / divideFactor);
        float zPositionPositive = (streetLength / 2);

        glBegin(GL_QUADS);
            // Desenhar o chão da rua
            glVertex3f(offsetX - streetWidth / 2, 0.0f, zPositionNegative);
            glVertex3f(offsetX + streetWidth / 2, 0.0f, zPositionNegative);
            glVertex3f(offsetX + streetWidth / 2, 0.0f, zPositionPositive);
            glVertex3f(offsetX - streetWidth / 2, 0.0f, zPositionPositive);
        glEnd();
    }

    // Desenhar ruas conectando as paralelas nas extremidades (frente e trás)
    glColor3f(1.0f, 0.5f, 1.0f); // Cor para as ruas de conexão
    glBegin(GL_QUADS);
        // Conexão na frente (parte frontal) com largura total das ruas paralelas
        glVertex3f(-totalParallelWidth / 2, 0.0f, (streetLength / 2) + streetWidth); // Vértice 1
        glVertex3f(-totalParallelWidth / 2, streetHeight, (streetLength / 2) + streetWidth); // Vértice 2
        glVertex3f(totalParallelWidth / 2, streetHeight, (streetLength / 2) + streetWidth); // Vértice 3
        glVertex3f(totalParallelWidth / 2, 0.0f, (streetLength / 2) + streetWidth); // Vértice 4

        // Conexão atrás (parte traseira) com largura total das ruas paralelas
        glVertex3f(-totalParallelWidth / 2, 0.0f, -(streetLength / 2) - streetWidth); // Vértice 5
        glVertex3f(-totalParallelWidth / 2, streetHeight, -(streetLength / 2) - streetWidth); // Vértice 6
        glVertex3f(totalParallelWidth / 2, streetHeight, -(streetLength / 2) - streetWidth); // Vértice 7
        glVertex3f(totalParallelWidth / 2, 0.0f, -(streetLength / 2) - streetWidth); // Vértice 8
    glEnd();

    // Desenhar o chão das conexões (frente e trás)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        // Chão da conexão da frente
        glVertex3f(-totalParallelWidth / 2, 0.0f, streetLength / 2);
        glVertex3f(totalParallelWidth / 2, 0.0f, streetLength / 2);
        glVertex3f(totalParallelWidth / 2, 0.0f, streetLength / 2 + streetWidth);
        glVertex3f(-totalParallelWidth / 2, 0.0f, streetLength / 2 + streetWidth);

        // Chão da conexão de trás
        glVertex3f(-totalParallelWidth / 2, 0.0f, -streetLength / 2);
        glVertex3f(totalParallelWidth / 2, 0.0f, -streetLength / 2);
        glVertex3f(totalParallelWidth / 2, 0.0f, -streetLength / 2 - streetWidth);
        glVertex3f(-totalParallelWidth / 2, 0.0f, -streetLength / 2 - streetWidth);
    glEnd();

    // glColor3f(1.0f, 0.2f, 0.2f);
    // glPointSize(10.0f); // Definir tamanho do ponto
    // glBegin(GL_POINTS);
    //     glVertex3f(totalParallelWidth / 2, 0.0f, streetLength / 2);
    // glEnd();
}

void drawInfiniteLines(){
    glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    glBegin(GL_LINES);
        glVertex3f(-1000.0f, 0.0f, 0.0f); // Extremo negativo do eixo X
        glVertex3f(1000.0f, 0.0f, 0.0f);  // Extremo positivo do eixo X
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // Verde
    glBegin(GL_LINES);
        glVertex3f(0.0f, -1000.0f, 0.0f); // Extremo negativo do eixo Y
        glVertex3f(0.0f, 1000.0f, 0.0f);  // Extremo positivo do eixo Y
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f); // Azul
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -1000.0f); // Extremo negativo do eixo Z
        glVertex3f(0.0f, 0.0f, 1000.0f);  // Extremo positivo do eixo Z
    glEnd();
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


int main(void) {
    // Inicializar o GLFW
    if (!glfwInit())
        return -1;

    // Criar uma janela GLFW
    GLFWwindow* window = glfwCreateWindow(1024, 768, "3D Scene", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback); // Registrar o callback do mouse
    initSmoothRendering(); // Habilitar a suavização

    // Desabilitar o cursor e centralizar
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Limpar o buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);  // Habilitar profundidade

        // Definir a projeção em perspectiva
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 1024.0 / 768.0, 0.1, 600.0);

        // Configurar a câmera (visão)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Direção da câmera
        float frontX = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        float frontY = sin(glm::radians(cameraPitch));
        float frontZ = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

        gluLookAt(cameraX, cameraY, cameraZ, cameraX + frontX, cameraY + frontY, cameraZ + frontZ, 0.0f, 1.0f, 0.0f);

        // Desenhar as paredes e o chão
        drawInfiniteLines();
        drawStreets();

        // Trocar os buffers e processar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Processar entrada do teclado
        processInput(window);
    }

    glfwTerminate();
    return 0;
}
