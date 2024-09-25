#include "main.h"
#include "../../utils/calculateWorldCoordinates/main.h"
#include "../../utils/ilumination/phong_directional/main.h"

void createStreet(
    float length, 
    float width, 
    bool drawLeftWall, 
    bool drawRightWall, 
    float leftWallLengthFactor, 
    float rightWallLengthFactor, 
    float wallHeight, 
    int streetIndex, 
    int closestStreetIndex, 
    float closestPointPercentage, 
    std::vector<SelectedPoint> selectedPoints, 
    const Light& light, 
    const Light& light_spot, 
    const Material& material, 
    const Camera& camera,
    glm::vec3 normalFloor,
    glm::vec3 normalLeftWall,
    glm::vec3 normalRightWall
) {
    glBegin(GL_TRIANGLES);
    int numSegments = 300;

    for (int i = 0; i < numSegments; ++i) {
        float t0 = (float)i / numSegments;
        float t1 = (float)(i + 1) / numSegments;

        // Triângulo 1 (Piso)
        glm::vec3 p0 = glm::vec3(-length / 2 + t0 * length, 0.0f, -width / 2);
        glm::vec3 p1 = glm::vec3(-length / 2 + t1 * length, 0.0f, -width / 2);
        glm::vec3 p2 = glm::vec3(-length / 2 + t0 * length, 0.0f, width / 2);

        glm::vec3 color = shading(p0, normalFloor, light, material, camera);
        glm::vec3 color_spot = shading_spot(p0, normalFloor, light_spot, material, camera, 65.0f);

        glm::vec3 finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p0
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p0.x, p0.y, p0.z);

        color = shading(p1, normalFloor, light, material, camera);
        color_spot = shading_spot(p1, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p1
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p1.x, p1.y, p1.z);

        color = shading(p2, normalFloor, light, material, camera);
        color_spot = shading_spot(p2, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p2
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p2.x, p2.y, p2.z);

        // Triângulo 2 (Piso)
        p0 = glm::vec3(-length / 2 + t1 * length, 0.0f, -width / 2);
        p1 = glm::vec3(-length / 2 + t1 * length, 0.0f, width / 2);
        p2 = glm::vec3(-length / 2 + t0 * length, 0.0f, width / 2);

        color = shading(p0, normalFloor, light, material, camera);
        color_spot = shading_spot(p0, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p0
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p0.x, p0.y, p0.z);

        color = shading(p1, normalFloor, light, material, camera);
        color_spot = shading_spot(p1, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p1
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p1.x, p1.y, p1.z);

        color = shading(p2, normalFloor, light, material, camera);
        color_spot = shading_spot(p2, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glNormal3f(normalFloor.x, normalFloor.y, normalFloor.z); // Define a normal para o vértice p2
        glColor3f(0.3f*finalColor.r, 0.3f*finalColor.g, 0.3f*finalColor.b);
        glVertex3f(p2.x, p2.y, p2.z);
    }
    glEnd();

    // Paredes (esquerda e direita)
    if (drawLeftWall) {
        float leftWallLength = length * leftWallLengthFactor;
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < numSegments; ++i) {
            float t0 = (float)i / numSegments;
            float t1 = (float)(i + 1) / numSegments;

            // Triângulo 1 (Parede Esquerda)
            glm::vec3 p0 = glm::vec3(-length / 2 + t0 * leftWallLength, 0.0f, -width / 2);
            glm::vec3 p1 = glm::vec3(-length / 2 + t1 * leftWallLength, 0.0f, -width / 2);
            glm::vec3 p2 = glm::vec3(-length / 2 + t0 * leftWallLength, wallHeight, -width / 2);

            glm::vec3 color = shading(p0, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p0.x, p0.y, p0.z);

            color = shading(p1, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p1.x, p1.y, p1.z);

            color = shading(p2, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p2.x, p2.y, p2.z);

            // Triângulo 2 (Parede Esquerda)
            p0 = glm::vec3(-length / 2 + t1 * leftWallLength, 0.0f, -width / 2);
            p1 = glm::vec3(-length / 2 + t1 * leftWallLength, wallHeight, -width / 2);
            p2 = glm::vec3(-length / 2 + t0 * leftWallLength, wallHeight, -width / 2);

            color = shading(p0, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p0.x, p0.y, p0.z);

            color = shading(p1, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p1.x, p1.y, p1.z);

            color = shading(p2, normalLeftWall, light, material, camera);
            glNormal3f(normalLeftWall.x, normalLeftWall.y, normalLeftWall.z);  // Define a normal
            glColor3f(color.r, color.g, color.b);
            glVertex3f(p2.x, p2.y, p2.z);
        }
        glEnd();
    }

    if (drawRightWall) {
        float rightWallLength = length * rightWallLengthFactor;
        int numTriangles = 20;  // Número de triângulos para formar a parede

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < numTriangles; ++i) {
            float t0 = (float)i / numTriangles;
            float t1 = (float)(i + 1) / numTriangles;

            glm::vec3 point0 = glm::vec3(-length / 2 + t0 * rightWallLength, 0.0f, width / 2);
            glm::vec3 point1 = glm::vec3(-length / 2 + t1 * rightWallLength, 0.0f, width / 2);
            glm::vec3 point2 = glm::vec3(-length / 2 + t0 * rightWallLength, wallHeight, width / 2);
            glm::vec3 point3 = glm::vec3(-length / 2 + t1 * rightWallLength, wallHeight, width / 2);

            // Primeiro triângulo da parede
            glm::vec3 color0 = shading(point0, normalRightWall, light, material, camera);
            glm::vec3 color1 = shading(point1, normalRightWall, light, material, camera);
            glm::vec3 color2 = shading(point2, normalRightWall, light, material, camera);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color0.r * 0.6f, color0.g * 0.6f, color0.b * 0.6f);
            glVertex3f(point0.x, point0.y, point0.z);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color1.r * 0.6f, color1.g * 0.6f, color1.b * 0.6f);
            glVertex3f(point1.x, point1.y, point1.z);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color2.r * 0.6f, color2.g * 0.6f, color2.b * 0.6f);
            glVertex3f(point2.x, point2.y, point2.z);

            // Segundo triângulo da parede
            glm::vec3 color3 = shading(point3, normalRightWall, light, material, camera);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color1.r * 0.6f, color1.g * 0.6f, color1.b * 0.6f);
            glVertex3f(point1.x, point1.y, point1.z);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color2.r * 0.6f, color2.g * 0.6f, color2.b * 0.6f);
            glVertex3f(point2.x, point2.y, point2.z);

            glNormal3f(normalRightWall.x, normalRightWall.y, normalRightWall.z);  // Define a normal
            glColor3f(color3.r * 0.6f, color3.g * 0.6f, color3.b * 0.6f);
            glVertex3f(point3.x, point3.y, point3.z);
        }
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
void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex, std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Light& light_spot, const Material& material, const Camera& camera, glm::vec3 normalFloor, glm::vec3 normalLeftWall, glm::vec3 normalRightWall) {
    // Desenhar a rua com iluminação Phong
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight, streetIndex, closestStreetIndex, closestPointPercentage, selectedPoints, light, light_spot, material, camera, normalFloor, normalLeftWall, normalRightWall);

    // Calcular coordenadas de mundo para a rua
    calculateWorldCoordinates(streetIndex, transformMatrix, length, worldCoordinates);
}
