#include "main.h"
#include "../../utils/calculateWorldCoordinates/main.h"
#include "../../utils/ilumination/phong_directional/main.h"

// Função que desenha a rua e aplica iluminação Phong nos vértices
void createStreet(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, int streetIndex, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Material& material, const Camera& camera) {
    glm::vec3 normal(0.0f, 1.0f, 0.0f);  // Normal constante para superfícies planas

    glBegin(GL_QUADS);
        // Aplicar a iluminação Phong em cada vértice
        for (int i = 0; i < 4; ++i) {
            glm::vec3 pointPosition;
            switch (i) {
                case 0: pointPosition = glm::vec3(-length / 2, 0.0f, -width / 2); break;
                case 1: pointPosition = glm::vec3(length / 2, 0.0f, -width / 2); break;
                case 2: pointPosition = glm::vec3(length / 2, 0.0f, width / 2); break;
                case 3: pointPosition = glm::vec3(-length / 2, 0.0f, width / 2); break;
            }

            // Aplicar a cor calculada
            glColor3f(0.3f, 0.3f, 0.3f);
            glVertex3f(pointPosition.x, pointPosition.y, pointPosition.z);
        }
    glEnd();

    if (drawLeftWall) {
        glColor3f(0.6f, 0.6f, 0.6f);
        float leftWallLength = length * leftWallLengthFactor;
        glBegin(GL_QUADS);
            glVertex3f(-length / 2, 0.0f, -width / 2);
            glVertex3f(-length / 2 + leftWallLength, 0.0f, -width / 2);
            glVertex3f(-length / 2 + leftWallLength, wallHeight, -width / 2);
            glVertex3f(-length / 2, wallHeight, -width / 2);
        glEnd();
    }

    if (drawRightWall) {
        glColor3f(0.6f, 0.6f, 0.6f);
        float rightWallLength = length * rightWallLengthFactor;
        glBegin(GL_QUADS);
            glVertex3f(-length / 2, 0.0f, width / 2);
            glVertex3f(-length / 2 + rightWallLength, 0.0f, width / 2);
            glVertex3f(-length / 2 + rightWallLength, wallHeight, width / 2);
            glVertex3f(-length / 2, wallHeight, width / 2);
        glEnd();
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(-length / 2, 0.1f, 0.0f);
        glVertex3f(length / 2, 0.1f, 0.0f);
    glEnd();


    float step = length / 4;
    float pointHeight = 0.2f;
    glPointSize(10.0f);

    std::vector<float> steps = {0, 25, 50, 75, 100};
    for (int i = 0; i < steps.size(); ++i) {
        float stepPosition = steps[i];
        bool isSelected = false;

        // Verificar se o ponto é um dos dois selecionados
        if(selectedPoints.size()){
            for (int j = 0; j < 2; ++j) {
                if (selectedPoints[j].streetIndex == streetIndex && selectedPoints[j].pointPercentage == stepPosition) {
                    isSelected = true;
                    break;
                }
            }
        }

        if (isSelected) {
            glColor3f(1.0f, 1.0f, 1.0f);  
        } else {
            switch (i) {
                case 0: glColor3f(1.0f, 0.0f, 0.0f); break;
                case 1: glColor3f(0.0f, 1.0f, 0.0f); break;  
                case 2: glColor3f(0.0f, 0.0f, 1.0f); break;  
                case 3: glColor3f(1.0f, 1.0f, 0.0f); break;  
                case 4: glColor3f(1.0f, 0.0f, 1.0f); break;  
            }
        }

        glBegin(GL_POINTS);
            glVertex3f(-length / 2 + step * i, pointHeight, 0.0f);
        glEnd();
    }
}

// Função que desenha a rua com suas coordenadas de mundo e aplica iluminação
void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex, std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Material& material, const Camera& camera) {
    // Desenhar a rua com iluminação Phong
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight, streetIndex, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);

    // Calcular coordenadas de mundo para a rua
    calculateWorldCoordinates(streetIndex, transformMatrix, length, worldCoordinates);
}
