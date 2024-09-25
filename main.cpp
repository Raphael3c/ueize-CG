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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "drawsModule/drawInfiniteLines/main.h"
#include "drawsModule/drawStreets/main.h"
#include "drawsModule/drawLineFollowingStreetTopology/main.h"
#include "drawsModule/drawLine/main.h"
#include "utils/findPathBetweenStreets/main.h"
#include "utils/screenToWorldRay/main.h"

unsigned int temporary_global_id;
unsigned int texId1;
unsigned int texId2;

const char* arquivo1 = "./textures/asfalto.jpg";
const char* arquivo2 = "./textures/asfalto2.jpg";
int largura, altura, nCanais;

std::unordered_map<int, StreetPoints> worldCoordinates;
std::unordered_map<int, std::vector<int>> streetConnections;

float cameraX = 6.12207127f, cameraY = 289.141296f, cameraZ = 40.0386086f;
float cameraYaw = glm::degrees(atan2(-cameraZ, -cameraX));
float cameraPitch = glm::degrees(atan2(-cameraY, sqrt(cameraX * cameraX + cameraZ * cameraZ)));
float cameraSpeed = 2.0f, mouseSensitivity = 0.1f;
float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true, controlCamera = true;

int currentCameraIndex = 0;
bool isCameraMoving = false;
std::vector<glm::vec3> cameraPath;
float cameraMoveSpeed = 0.5f;

// Variáveis de backup da câmera
float cameraXBP = 6.12207127f, cameraYBP = 289.141296f, cameraZBP = 40.0386086f;

bool lanternEnabled = true;
bool sunEnabled = true;     
bool sunRotationEnabled = false;  
float sunRotationAngle = 0.0f;   
const float SUN_ROTATION_SPEED = 45.0f;  

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

// Luz direcional
Light sun = {
    glm::vec3(0.8f),   
    glm::vec3(1.0f, 1.0f, 1.0f),  
    glm::vec3(1.0f, 1.0f, 1.0f), 
    glm::normalize(-glm::vec3(-0.2f, -1.0f, -0.3f)), 
    glm::vec3(0.0f, 40.0f, 0.0f) 
};

void updateCameraPosition() {
    if (isCameraMoving && currentCameraIndex < cameraPath.size()) {
        glm::vec3 targetPoint = cameraPath[currentCameraIndex];

        glm::vec3 direction = targetPoint - glm::vec3(cameraX, cameraY, cameraZ);

        if (glm::length(direction) > 0.001f) {
            direction = glm::normalize(direction);  
            glm::vec3 step = direction * cameraMoveSpeed;

            cameraX += step.x;
            cameraY += step.y; 
            cameraZ += step.z;
        }

        if (glm::length(glm::vec3(cameraX, cameraY, cameraZ) - targetPoint) < 0.5f) {
            currentCameraIndex++;
        }

        if (currentCameraIndex >= cameraPath.size()) {
            isCameraMoving = false;

            cameraX = cameraXBP;
            cameraY = cameraYBP;
            cameraZ = cameraZBP;

            selectedPoints = std::vector<SelectedPoint>(2, {0, 0.0f});
        }
    }
}

void updateSunPosition(float deltaTime) {
    if (sunRotationEnabled) {
        sunRotationAngle += SUN_ROTATION_SPEED * deltaTime; 
        if (sunRotationAngle > 360.0f) {
            sunRotationAngle -= 360.0f; 
        }
        
        sun.direction = glm::normalize(glm::vec3(cos(glm::radians(sunRotationAngle)), sin(glm::radians(sunRotationAngle)), -0.3f));
    }
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
            selectedPoints[pointCount % 2] = {closestStreetIndex, closestPointPercentage};
            pointCount++;

            if (pointCount >= 2) {
                lineCoordinates.clear();  
                drawLineFollowingStreetTopology(selectedPoints[0], selectedPoints[1], streetConnections);

                cameraPath = lineCoordinates;
                currentCameraIndex = 0;
                isCameraMoving = true;

                if (!cameraPath.empty()) {
                    cameraX = cameraPath[0].x;
                    cameraY = cameraPath[0].y + 20.0f;
                    cameraZ = cameraPath[0].z;

                    glm::vec3 directionToOrigin = glm::normalize(glm::vec3(-cameraX, -cameraY, -cameraZ));
                    cameraYaw = glm::degrees(atan2(directionToOrigin.z, directionToOrigin.x));
                    cameraPitch = glm::degrees(asin(directionToOrigin.y));
                }
            }
        }
    }
}

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

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        lanternEnabled = !lanternEnabled;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        sunEnabled = !sunEnabled;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        sun.position.y = -sun.position.y;
    }
}

void initSmoothRendering() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void loadTexture(const char* path) {
    stbi_set_flip_vertically_on_load(true);

    unsigned char *dados = stbi_load(path, &largura, &altura, &nCanais, 0);
    unsigned int temporary;

    if(!dados) printf("Textura não pode ser carregada\n");

    glGenTextures(1, &temporary);
    glBindTexture(GL_TEXTURE_2D, temporary);

    temporary_global_id = temporary;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::cout << largura << " " << altura << std::endl;

    GLenum format = (nCanais == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(dados);
}

int main(void) {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Trabalho CG", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 100, 100);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    loadTexture(arquivo1);
    texId1 = temporary_global_id;
    loadTexture(arquivo2);
    texId2 = temporary_global_id;

    initSmoothRendering();
    initializeStreetConnections();

    float lastFrameTime = glfwGetTime();  
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        float aspectRatio = (float)windowWidth / (float)windowHeight;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspectRatio, 0.1, 1000.0); 

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;


        updateCameraPosition();

        updateSunPosition(deltaTime);

        glm::vec3 cameraFront(
            cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
            sin(glm::radians(cameraPitch)),
            sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)));

        glm::vec3 cameraPos(cameraX, cameraY, cameraZ);
        glm::vec3 cameraTarget = cameraPos + cameraFront;

        gluLookAt(cameraX, cameraY + 10.0f, cameraZ, cameraTarget.x, cameraTarget.y + 10.0f, cameraTarget.z, 0.0f, 1.0f, 0.0f);

        Camera camera = { glm::vec3(cameraX, cameraY, cameraZ) };

        glm::vec3 offset = glm::vec3(0.0f, 3.0f, 0.0f); 
        glm::vec3 behind = -glm::normalize(cameraTarget - camera.position) * 30.0f;

        Light lantern = {
                glm::vec3(0.0f),  
                glm::vec3(0.0f),     
                glm::vec3(0.0f),   
                glm::normalize(camera.position),     
                camera.position + offset + behind
            };

        if (lanternEnabled) {
            lantern = {
                glm::vec3(0.5f, 0.5f, 0.5f),  
                glm::vec3(1.0f, 1.0f, 0.7f),     
                glm::vec3(1.0f, 1.0f, 0.8f),   
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

            glPointSize(10.0f);

            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f); 
            glVertex3f(lantern.position.x, lantern.position.y, lantern.position.z);
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

        drawStreets(
            worldCoordinates, 
            closestStreetIndex, 
            closestPointPercentage, 
            selectedPoints, 
            camera, 
            sun, 
            lantern,
            texId1,
            texId2
        );

        // drawInfiniteLines();
        drawLine();

        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
    }

    glfwTerminate();
    return 0;
}
