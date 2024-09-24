#include "main.h"
#include "../../objectsModule/createStreet/main.h"

Light light = {
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(0.9f, 0.9f, 0.9f),  // ambient
    glm::vec3(0.8f, 0.8f, 0.8f),  // diffuse
    glm::vec3(1.0f, 1.0f, 1.0f)   // specular
};

Material material = {
    glm::vec3(0.6f, 0.6f, 0.6f),  // ambient
    glm::vec3(0.8f, 0.8f, 0.8f),  // diffuse
    glm::vec3(0.3f, 0.3f, 0.3f),  // specular
    3.0f                         // shininess
};

void drawStreets(std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage, std::vector<SelectedPoint> selectedPoints, Camera camera) {
    float streetLength = 200.0f;  
    float streetWidth = 30.0f;   
    float streetHeight = 6.0f;    
    float gapBetweenStreets = 20.0f; 
    int numStreets = 4;  

    float xOffset = (streetWidth + gapBetweenStreets);
    
    glm::mat4 model = glm::mat4(1.0f); 
    int streetIndex = 0;

    // Normal das ruas (superfícies planas apontando para cima)
    glm::vec3 normal(0.0f, 1.0f, 0.0f);  

    //Rua 0
    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, (streetWidth/2) + (streetLength/2)));
        glMultMatrixf(glm::value_ptr(model));

        // Passando os novos parâmetros de iluminação
        createStreetWithWorldCoordinates(streetLength * 2, streetWidth, false, true, 1.0f, 1.0f, streetHeight, 
                                    model, streetIndex++, worldCoordinates, closestStreetIndex, 
                                    closestPointPercentage, selectedPoints, light, material, camera);
            
        model = glm::mat4(1.0f);    
    glPopMatrix();

    glPointSize(15.0f);

    // Definir a cor do ponto (por exemplo, amarelo para representar o Sol)
    glColor3f(1.0f, 1.0f, 0.0f);
    
    // Desenhar o ponto na posição da origem visual da luz
    glBegin(GL_POINTS);
        glVertex3f(0.0f, 10.0f, 0.0f);
    glEnd();


    //Rua 1
    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -((streetWidth/2) + (streetLength/2))));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength*2, streetWidth, true, false, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f); 
    glPopMatrix();
    
    //______________________________

    //Rua 2
    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -xOffset*1));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    //Rua 3

    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glMultMatrixf(glm::value_ptr(model)); 
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    //Ruas da base de piramide
    //Rua 4
    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, xOffset*1));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 0.685f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f);
    glPopMatrix();

    //Rua 5
    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 shearMatrix = glm::mat4(1.0f);
        shearMatrix[0][2] = 0.2f; 
        model = model * shearMatrix;
        model = glm::translate(model, glm::vec3(((-streetLength/2) - (gapBetweenStreets + (streetWidth/2) + streetWidth)), 0.0f, (xOffset*1 + 15.0f))); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, false, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f);
    glPopMatrix();

    //Rua 6
    glPushMatrix();
        model = glm::translate(model, glm::vec3(-streetLength*0.1625, 0.0f, xOffset*2));
        glMultMatrixf(glm::value_ptr(model));  
        createStreetWithWorldCoordinates(streetLength*0.675, streetWidth, true, true, 0.985f, 0.941f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage, selectedPoints, light, material, camera);
        model = glm::mat4(1.0f);
    glPopMatrix();
}
