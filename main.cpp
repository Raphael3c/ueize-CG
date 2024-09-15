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

// Estrutura para armazenar pontos de uma rua
struct StreetPoints {
    std::unordered_map<float, glm::vec3> points;
};

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

// Função para alternar entre controle de câmera e cursor
void toggleCameraControl() {
    controlCamera = !controlCamera;
}

// Função para desenhar o cursor personalizado
void drawCustomCursor() {
    glColor3f(1.0f, 0.0f, 0.0f);  // Cor vermelha
    glPointSize(10.0f);
    glBegin(GL_POINTS);
        glVertex2f(customCursorX, customCursorY);
    glEnd();
}

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
        glm::mat4 viewMatrix = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), 
                                           glm::vec3(cameraX + cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
                                                     cameraY + sin(glm::radians(cameraPitch)),
                                                     cameraZ + sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch))),
                                           glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 ray = screenToWorldRay((int)mouseX, (int)mouseY, windowWidth, windowHeight, projectionMatrix, viewMatrix);

        // Verificar a interseção com os pontos
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

        // Se um ponto mais próximo foi encontrado, ele será destacado em branco
        if (closestStreetIndex != -1) {
            std::cout << "Ponto mais próximo: Rua " << closestStreetIndex 
                      << ", Posição " << closestPointPercentage << "%\n";
        }
    }
}

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

// Função de callback para o movimento do mouse
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;

    // Se estiver controlando o cursor personalizado
    if (!controlCamera) {
        // Normalizar as coordenadas do cursor (entre -1 e 1) na tela
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        customCursorX = (xpos / windowWidth) * 2.0f - 1.0f;
        customCursorY = 1.0f - (ypos / windowHeight) * 2.0f;
        return;
    }

    // Se estiver controlando a câmera
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;  // Coordenadas Y invertidas

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

// Função para processar entrada do teclado e mover a câmera
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        toggleCameraControl();
        // Alternar o controle do cursor do sistema
        if (!controlCamera) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Exibir cursor do sistema
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Desabilitar cursor do sistema
        }
    }

    // Controlar a câmera apenas se o controle de câmera estiver ativo
    if (controlCamera) {
        glm::vec3 cameraFront;
        cameraFront.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront.y = sin(glm::radians(cameraPitch));
        cameraFront.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        cameraFront = glm::normalize(cameraFront);  // Normalizar o vetor para garantir tamanho 1

        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f))); // Vetor da direita
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront)); // Vetor para cima

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

// Função que desenha a rua e os pontos, mudando a cor do ponto mais próximo
void createStreet(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, int streetIndex) {
    // Desenhar a rua na origem (cor cinza escuro)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex3f(-length / 2, 0.0f, -width / 2);  
        glVertex3f(length / 2, 0.0f, -width / 2);   
        glVertex3f(length / 2, 0.0f, width / 2);    
        glVertex3f(-length / 2, 0.0f, width / 2);   
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

    // Desenhar os pontos
    float step = length / 4;
    float pointHeight = 0.2f;
    glPointSize(10.0f);

    std::vector<float> steps = {0, 25, 50, 75, 100};
    for (int i = 0; i < steps.size(); ++i) {
        float stepPosition = steps[i];
        // Verifique se o ponto atual é o mais próximo
        if (closestStreetIndex == streetIndex && closestPointPercentage == stepPosition) {
            glColor3f(1.0f, 1.0f, 1.0f);  // Branco se for o ponto mais próximo
        } else {
            switch (i) {
                case 0: glColor3f(1.0f, 0.0f, 0.0f); break;  // Vermelho
                case 1: glColor3f(0.0f, 1.0f, 0.0f); break;  // Verde
                case 2: glColor3f(0.0f, 0.0f, 1.0f); break;  // Azul
                case 3: glColor3f(1.0f, 1.0f, 0.0f); break;  // Amarelo
                case 4: glColor3f(1.0f, 0.0f, 1.0f); break;  // Roxo
            }
        }

        glBegin(GL_POINTS);
            glVertex3f(-length / 2 + step * i, pointHeight, 0.0f);
        glEnd();
    }
}

// Função que desenha a rua com suas coordenadas de mundo
void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex) {
    // Desenhar a rua e calcular suas coordenadas de mundo
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight, streetIndex); 

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

       // Desenhar o cursor personalizado
        if (!controlCamera) {
            drawCustomCursor();
        }
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
        drawStreets();        // Chamada para desenhar as ruas
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
