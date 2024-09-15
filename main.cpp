#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Para funções de transformação de matriz como glm::perspective
#include <glm/gtc/type_ptr.hpp>          // Para usar glm::value_ptr se necessário

#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip> 

struct StreetPoints {
    std::unordered_map<float, glm::vec3> points;
};

std::unordered_map<int, StreetPoints> worldCoordinates;

// Variáveis globais para a posição e orientação da câmera
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = 3.0f;  // Posição inicial da câmera
float cameraYaw = -90.0f, cameraPitch = 0.0f;           // Direção da câmera
float cameraSpeed = 0.5f;                              // Velocidade de movimento
float mouseSensitivity = 0.1f;
float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true;
bool cameraLocked = false;

bool printOnlyOne = true;
// Variáveis de posição do mouse
double mouseX = 0.0;
double mouseY = 0.0;

// Função que calcula as coordenadas no mundo de cada ponto da rua
void calculateWorldCoordinates(int streetIndex, glm::mat4 transformMatrix, float length) {
    float step = length / 4;  // Divisão em 4 partes

    // Posições locais dos pontos (antes das transformações)
    std::vector<glm::vec3> localPoints = {
        glm::vec3(-length / 2, 0.2f, 0.0f),        // 0%
        glm::vec3(-length / 2 + step, 0.2f, 0.0f), // 25%
        glm::vec3(-length / 2 + step * 2, 0.2f, 0.0f), // 50%
        glm::vec3(-length / 2 + step * 3, 0.2f, 0.0f), // 75%
        glm::vec3(-length / 2 + step * 4, 0.2f, 0.0f)  // 100%
    };

    // Aplicar a matriz de transformação para calcular as coordenadas de mundo
    StreetPoints streetPoints;
    std::vector<float> stepPercentages = {0, 25, 50, 75, 100};

    for (int i = 0; i < localPoints.size(); ++i) {
        glm::vec4 transformedPoint = transformMatrix * glm::vec4(localPoints[i], 1.0f);
        streetPoints.points[stepPercentages[i]] = glm::vec3(transformedPoint);
    }

    // Armazenar as coordenadas de mundo para esta rua
    worldCoordinates[streetIndex] = streetPoints;
}

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

void createStreet(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight) {
    // Desenhar a rua na origem (cor cinza escuro)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        // Vértices da rua (centrada na origem)
        glVertex3f(-length / 2, 0.0f, -width / 2);  // Vértice 1: canto inferior esquerdo
        glVertex3f(length / 2, 0.0f, -width / 2);   // Vértice 2: canto inferior direito
        glVertex3f(length / 2, 0.0f, width / 2);    // Vértice 3: canto superior direito
        glVertex3f(-length / 2, 0.0f, width / 2);   // Vértice 4: canto superior esquerdo
    glEnd();

    // Desenhar o muro da esquerda (se `drawLeftWall` for verdadeiro)
    if (drawLeftWall) {
        glColor3f(0.6f, 0.6f, 0.6f);  // Cor cinza claro para o muro
        float leftWallLength = length * leftWallLengthFactor; // Comprimento do muro da esquerda
        glBegin(GL_QUADS);
            // Muro da esquerda
            glVertex3f(-length / 2, 0.0f, -width / 2);         // Canto inferior esquerdo da rua
            glVertex3f(-length / 2 + leftWallLength, 0.0f, -width / 2);   // Canto inferior direito do muro
            glVertex3f(-length / 2 + leftWallLength, wallHeight, -width / 2); // Canto superior direito do muro
            glVertex3f(-length / 2, wallHeight, -width / 2);   // Canto superior esquerdo do muro
        glEnd();
    }

    // Desenhar o muro da direita (se `drawRightWall` for verdadeiro)
    if (drawRightWall) {
        glColor3f(0.6f, 0.6f, 0.6f);  // Cor cinza claro para o muro
        float rightWallLength = length * rightWallLengthFactor; // Comprimento do muro da direita
        glBegin(GL_QUADS);
            // Muro da direita
            glVertex3f(-length / 2, 0.0f, width / 2);          // Canto inferior esquerdo da rua
            glVertex3f(-length / 2 + rightWallLength, 0.0f, width / 2);           // Canto inferior direito do muro
            glVertex3f(-length / 2 + rightWallLength, wallHeight, width / 2);      // Canto superior direito do muro
            glVertex3f(-length / 2, wallHeight, width / 2);     // Canto superior esquerdo do muro
        glEnd();
    }

    // Desenhar uma linha no meio da rua
    glColor3f(1.0f, 1.0f, 1.0f);  // Cor branca para a linha
    glBegin(GL_LINES);
        glVertex3f(-length / 2, 0.1f, 0.0f);  // Ponto inicial da linha no centro da rua
        glVertex3f(length / 2, 0.1f, 0.0f);   // Ponto final da linha no centro da rua
    glEnd();

    // Desenhar os pontos ao longo da linha
    float step = length / 4;  // Divisão em 4 partes
    float pointHeight = 0.2f;  // Altura dos pontos (levemente acima da linha)

    // Desenhar os 4 pontos ao longo da linha
    glPointSize(10.0f);  // Tamanho dos pontos

    // Ponto 0% (início)
    glColor3f(1.0f, 0.0f, 0.0f);  // Cor vermelha
    glBegin(GL_POINTS);
        glVertex3f(-length / 2, pointHeight, 0.0f);
    glEnd();

    // Ponto 25%
    glColor3f(0.0f, 1.0f, 0.0f);  // Cor verde
    glBegin(GL_POINTS);
        glVertex3f(-length / 2 + step, pointHeight, 0.0f);
    glEnd();

    // Ponto 50%
    glColor3f(0.0f, 0.0f, 1.0f);  // Cor azul
    glBegin(GL_POINTS);
        glVertex3f(-length / 2 + step * 2, pointHeight, 0.0f);
    glEnd();

    // Ponto 75%
    glColor3f(1.0f, 1.0f, 0.0f);  // Cor amarela
    glBegin(GL_POINTS);
        glVertex3f(-length / 2 + step * 3, pointHeight, 0.0f);
    glEnd();

    // Ponto 100%
    glColor3f(1.0f, 0.0f, 1.0f);  // Cor amarela
    glBegin(GL_POINTS);
        glVertex3f(-length / 2 + step * 4, pointHeight, 0.0f);
    glEnd();
}

void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex) {
    // Desenhar a rua e calcular suas coordenadas de mundo
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight); 

    // Calcular as coordenadas de mundo dos pontos após as transformações
    calculateWorldCoordinates(streetIndex, transformMatrix, length);
}

void drawStreets() {
    // Variáveis para controlar a largura e o comprimento das ruas
    float streetLength = 200.0f;  // Comprimento das ruas
    float streetWidth = 30.0f;    // Largura da rua
    float streetHeight = 6.0f;    // Altura das ruas (equivalente aos muros)
    float gapBetweenStreets = 20.0f; // Gap entre ruas paralelas
    int numStreets = 4;  // Número de ruas

    float xOffset = (streetWidth + gapBetweenStreets);
    
    glm::mat4 model = glm::mat4(1.0f); 
    int streetIndex = 0;

    //RUas principais

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, (streetWidth/2) + (streetLength/2)));
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength*2, streetWidth, false, true, 1.0f, 1.0f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f);    
    glPopMatrix();

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -((streetWidth/2) + (streetLength/2))));
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength*2, streetWidth, true, false, 1.0f, 1.0f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f); 
    glPopMatrix();
    
    //______________________________

    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -xOffset*1)); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    //Ruas da base de piramide
    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, xOffset*1)); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 0.685f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f);
    glPopMatrix();

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 shearMatrix = glm::mat4(1.0f);
        shearMatrix[0][2] = 0.2f;  // A quantidade de cisalhamento (ajuste conforme necessário)
        model = model * shearMatrix;
        model = glm::translate(model, glm::vec3(((-streetLength/2) - (gapBetweenStreets + (streetWidth/2) + streetWidth)), 0.0f, (xOffset*1 + 15.0f))); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength, streetWidth, false, true, 1.0f, 1.0f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f);
    glPopMatrix();

    glPushMatrix();
        model = glm::translate(model, glm::vec3(-streetLength*0.1625, 0.0f, xOffset*2)); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));  // Aplicar a matriz de transformação no OpenGL
        createStreetWithWorldCoordinates(streetLength*0.675, streetWidth, true, true, 0.985f, 0.941f, streetHeight, model, streetIndex++);
        model = glm::mat4(1.0f);
    glPopMatrix();
    //Ruas da base de piramide

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

void printWorldCoordinates() {
    if(!printOnlyOne) return;
    
    for (const auto& street : worldCoordinates) {
        std::cout << "Street " << street.first << " coordinates:\n";
        for (const auto& point : street.second.points) {
            std::cout << std::fixed << std::setprecision(2);  // Configura a saída com 2 casas decimais
            std::cout << "  " << point.first << "%: (" 
                      << point.second.x << ", " 
                      << point.second.y << ", " 
                      << point.second.z << ")\n";
        }
    }

    printOnlyOne = false;
}

int main(void) {
    // Inicializar o GLFW
    if (!glfwInit())
        return -1;

    // Desabilitar o modo fullscreen, configurando o monitor para NULL
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);  // Deixe a janela redimensionável, caso necessário

    // Criar uma janela GLFW com uma resolução específica (não fullscreen)
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Street Model with GLM", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Ajustar o posicionamento da janela para garantir que ela não abra maximizada ou em modo tela cheia
    glfwSetWindowPos(window, 100, 100);  // Posiciona a janela no canto superior esquerdo (ajuste conforme desejar)

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

        // Obter a largura e a altura da janela
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        // Definir a proporção da janela
        float aspectRatio = (float)windowWidth / (float)windowHeight;

        // Definir a projeção em perspectiva ajustada ao tamanho da tela
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspectRatio, 0.1, 600.0);

        // Configurar a câmera (visão)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Direção da câmera
        float frontX = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        float frontY = sin(glm::radians(cameraPitch));
        float frontZ = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

        gluLookAt(cameraX, cameraY, cameraZ, cameraX + frontX, cameraY + frontY, cameraZ + frontZ, 0.0f, 1.0f, 0.0f);

        // Desenhar as ruas
        drawStreets();
        drawInfiniteLines();
        printWorldCoordinates();

        // Trocar os buffers e processar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Processar entrada do teclado
        processInput(window);
    }

    glfwTerminate();
    return 0;
}

