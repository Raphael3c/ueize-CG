#include "main.h"
#include "../../utils/calculateWorldCoordinates/main.h"

// Função que desenha a rua e os pontos
void createStreet(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, int streetIndex, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints) {
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex3f(-length / 2, 0.0f, -width / 2);  
        glVertex3f(length / 2, 0.0f, -width / 2);   
        glVertex3f(length / 2, 0.0f, width / 2);    
        glVertex3f(-length / 2, 0.0f, width / 2);   
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

// Função que desenha a rua com suas coordenadas de mundo
void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex, std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints) {
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight, streetIndex, closestStreetIndex, closestPointPercentage, selectedPoints); 

    calculateWorldCoordinates(streetIndex, transformMatrix, length, worldCoordinates);
}
