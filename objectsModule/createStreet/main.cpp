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
    glm::vec3 normalRightWall,

    glm::mat4 model
) {
    glBegin(GL_TRIANGLES);
        int numSegments = 20;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model))); 

        for (int i = 0; i < numSegments; ++i) {
        float t0 = (float)i / numSegments;
        float t1 = (float)(i + 1) / numSegments;

        normalFloor = glm::normalize(normalMatrix * normalFloor);

        // Triângulo 1 (Piso)
        glm::vec3 p0 = glm::vec3(-length / 2 + t0 * length, 0.0f, -width / 2);
        glm::vec3 p1 = glm::vec3(-length / 2 + t1 * length, 0.0f, -width / 2);
        glm::vec3 p2 = glm::vec3(-length / 2 + t0 * length, 0.0f, width / 2);

        // Aplicando a matriz model
        p0 = glm::vec3(model * glm::vec4(p0, 1.0f));
        p1 = glm::vec3(model * glm::vec4(p1, 1.0f));
        p2 = glm::vec3(model * glm::vec4(p2, 1.0f));

        glm::vec3 color = shading(p0, normalFloor, light, material, camera);
        glm::vec3 color_spot = shading_spot(p0, normalFloor, light_spot, material, camera, 65.0f);
        glm::vec3 finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p0.x, p0.y, p0.z);

        color = shading(p1, normalFloor, light, material, camera);
        color_spot = shading_spot(p1, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p1.x, p1.y, p1.z);

        color = shading(p2, normalFloor, light, material, camera);
        color_spot = shading_spot(p2, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p2.x, p2.y, p2.z);

        // Triângulo 2 (Piso)
        p0 = glm::vec3(-length / 2 + t1 * length, 0.0f, -width / 2);
        p1 = glm::vec3(-length / 2 + t1 * length, 0.0f, width / 2);
        p2 = glm::vec3(-length / 2 + t0 * length, 0.0f, width / 2);

        // Aplicando a matriz model
        p0 = glm::vec3(model * glm::vec4(p0, 1.0f));
        p1 = glm::vec3(model * glm::vec4(p1, 1.0f));
        p2 = glm::vec3(model * glm::vec4(p2, 1.0f));

        color = shading(p0, normalFloor, light, material, camera);
        color_spot = shading_spot(p0, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p0.x, p0.y, p0.z);

        color = shading(p1, normalFloor, light, material, camera);
        color_spot = shading_spot(p1, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p1.x, p1.y, p1.z);

        color = shading(p2, normalFloor, light, material, camera);
        color_spot = shading_spot(p2, normalFloor, light_spot, material, camera, 65.0f);
        finalColor = color + color_spot;

        glColor3f(finalColor.r, finalColor.g, finalColor.b);
        glVertex3f(p2.x, p2.y, p2.z);
    }
    glEnd();

// Paredes (esquerda e direita)
    if (drawLeftWall) {
        float leftWallLength = length * leftWallLengthFactor;
        normalLeftWall = glm::normalize(normalMatrix * normalLeftWall);

        glBegin(GL_TRIANGLES);
            for (int i = 0; i < numSegments; ++i) {
                float t0 = (float)i / numSegments;
                float t1 = (float)(i + 1) / numSegments;

                // Triângulo 1 (Parede Esquerda)
                glm::vec3 p0 = glm::vec3(-length / 2 + t0 * leftWallLength, 0.0f, -width / 2);
                glm::vec3 p1 = glm::vec3(-length / 2 + t1 * leftWallLength, 0.0f, -width / 2);
                glm::vec3 p2 = glm::vec3(-length / 2 + t0 * leftWallLength, wallHeight, -width / 2);

                // Aplicando a matriz model
                p0 = glm::vec3(model * glm::vec4(p0, 1.0f));
                p1 = glm::vec3(model * glm::vec4(p1, 1.0f));
                p2 = glm::vec3(model * glm::vec4(p2, 1.0f));

                glm::vec3 color = shading(p0, normalLeftWall, light, material, camera);
                glm::vec3 color_spot = shading_spot(p0, normalLeftWall, light_spot, material, camera, 65.0f);
                glm::vec3 finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p0.x, p0.y, p0.z);

                color = shading(p1, normalLeftWall, light, material, camera);
                color_spot = shading_spot(p1, normalLeftWall, light_spot, material, camera, 65.0f);
                finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p1.x, p1.y, p1.z);

                color = shading(p2, normalLeftWall, light, material, camera);
                color_spot = shading_spot(p2, normalLeftWall, light_spot, material, camera, 65.0f);
                finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p2.x, p2.y, p2.z);

                // Triângulo 2 (Parede Esquerda)
                p0 = glm::vec3(-length / 2 + t1 * leftWallLength, 0.0f, -width / 2);
                p1 = glm::vec3(-length / 2 + t1 * leftWallLength, wallHeight, -width / 2);
                p2 = glm::vec3(-length / 2 + t0 * leftWallLength, wallHeight, -width / 2);

                // Aplicando a matriz model
                p0 = glm::vec3(model * glm::vec4(p0, 1.0f));
                p1 = glm::vec3(model * glm::vec4(p1, 1.0f));
                p2 = glm::vec3(model * glm::vec4(p2, 1.0f));

                color = shading(p0, normalLeftWall, light, material, camera);
                color_spot = shading_spot(p0, normalLeftWall, light_spot, material, camera, 65.0f);
                finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p0.x, p0.y, p0.z);

                color = shading(p1, normalLeftWall, light, material, camera);
                color_spot = shading_spot(p1, normalLeftWall, light_spot, material, camera, 65.0f);
                finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p1.x, p1.y, p1.z);

                color = shading(p2, normalLeftWall, light, material, camera);
                color_spot = shading_spot(p2, normalLeftWall, light_spot, material, camera, 65.0f);
                finalColor = color + color_spot;
                glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
                glVertex3f(p2.x, p2.y, p2.z);

        }
        glEnd();
    }

    if (drawRightWall) {
        float rightWallLength = length * rightWallLengthFactor;
        int numTriangles = 20;
        normalRightWall = glm::normalize(normalMatrix * normalRightWall);

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < numTriangles; ++i) {
            float t0 = (float)i / numTriangles;
            float t1 = (float)(i + 1) / numTriangles;

            glm::vec3 point0 = glm::vec3(-length / 2 + t0 * rightWallLength, 0.0f, width / 2);
            glm::vec3 point1 = glm::vec3(-length / 2 + t1 * rightWallLength, 0.0f, width / 2);
            glm::vec3 point2 = glm::vec3(-length / 2 + t0 * rightWallLength, wallHeight, width / 2);
            glm::vec3 point3 = glm::vec3(-length / 2 + t1 * rightWallLength, wallHeight, width / 2);

            // Aplicando a matriz model
            point0 = glm::vec3(model * glm::vec4(point0, 1.0f));
            point1 = glm::vec3(model * glm::vec4(point1, 1.0f));
            point2 = glm::vec3(model * glm::vec4(point2, 1.0f));
            point3 = glm::vec3(model * glm::vec4(point3, 1.0f));

            glm::vec3 color0 = shading(point0, normalRightWall, light, material, camera);
            glm::vec3 color1 = shading(point1, normalRightWall, light, material, camera);
            glm::vec3 color2 = shading(point2, normalRightWall, light, material, camera);
            glm::vec3 color3 = shading(point3, normalRightWall, light, material, camera);

            glm::vec3 color0_spot = shading_spot(point0, normalRightWall, light_spot, material, camera, 65.0f);
            glm::vec3 color1_spot = shading_spot(point1, normalRightWall, light_spot, material, camera, 65.0f);
            glm::vec3 color2_spot= shading_spot(point2, normalRightWall, light_spot, material, camera, 65.0f);
            glm::vec3 color3_spot= shading_spot(point3, normalRightWall, light_spot, material, camera, 65.0f);

            glm::vec3 finalColor = color0 + color0_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point0.x, point0.y, point0.z);

            finalColor = color1 + color1_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point1.x, point1.y, point1.z);

            finalColor = color2 + color2_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point2.x, point2.y, point2.z);

            finalColor = color3 + color3_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point3.x, point3.y, point3.z);

            finalColor = color1 + color1_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point1.x, point1.y, point1.z);

            finalColor = color2 + color2_spot;
            glColor3f(0.1f+finalColor.r, 0.1f+finalColor.g, 0.1f+finalColor.b);
            glVertex3f(point2.x, point2.y, point2.z);
        }
        glEnd();
    }

    glMultMatrixf(glm::value_ptr(model));

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(-length / 2, 0.1f, 0.0f);
        glVertex3f(length / 2, 0.1f, 0.0f);
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

void createStreetWithWorldCoordinates(float length, float width, bool drawLeftWall, bool drawRightWall, float leftWallLengthFactor, float rightWallLengthFactor, float wallHeight, glm::mat4 transformMatrix, int streetIndex, std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, const Light& light, const Light& light_spot, const Material& material, const Camera& camera, glm::vec3 normalFloor, glm::vec3 normalLeftWall, glm::vec3 normalRightWall) {
    createStreet(length, width, drawLeftWall, drawRightWall, leftWallLengthFactor, rightWallLengthFactor, wallHeight, streetIndex, closestStreetIndex, closestPointPercentage, selectedPoints, light, light_spot, material, camera, normalFloor, normalLeftWall, normalRightWall, transformMatrix);

    calculateWorldCoordinates(streetIndex, transformMatrix, length, worldCoordinates);
}
