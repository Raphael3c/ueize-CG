#include "main.h"
#include "../../objectsModule/createStreet/main.h"

void drawStreets(std::unordered_map<int, StreetPoints>& worldCoordinates, int closestStreetIndex, float closestPointPercentage) {
    float streetLength = 200.0f;  
    float streetWidth = 30.0f;   
    float streetHeight = 6.0f;    
    float gapBetweenStreets = 20.0f; 
    int numStreets = 4;  

    float xOffset = (streetWidth + gapBetweenStreets);
    
    glm::mat4 model = glm::mat4(1.0f); 
    int streetIndex = 0;

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, (streetWidth/2) + (streetLength/2)));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength*2, streetWidth, false, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f);    
    glPopMatrix();

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -((streetWidth/2) + (streetLength/2))));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength*2, streetWidth, true, false, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f); 
    glPopMatrix();
    
    //______________________________

    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -xOffset*1));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glMultMatrixf(glm::value_ptr(model)); 
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f); 
    glPopMatrix();

    //Ruas da base de piramide
    glPushMatrix();
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, xOffset*1));
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, true, true, 1.0f, 0.685f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f);
    glPopMatrix();

    glPushMatrix();
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 shearMatrix = glm::mat4(1.0f);
        shearMatrix[0][2] = 0.2f; 
        model = model * shearMatrix;
        model = glm::translate(model, glm::vec3(((-streetLength/2) - (gapBetweenStreets + (streetWidth/2) + streetWidth)), 0.0f, (xOffset*1 + 15.0f))); // Transladar a rua ao longo do eixo X
        glMultMatrixf(glm::value_ptr(model));
        createStreetWithWorldCoordinates(streetLength, streetWidth, false, true, 1.0f, 1.0f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f);
    glPopMatrix();

    glPushMatrix();
        model = glm::translate(model, glm::vec3(-streetLength*0.1625, 0.0f, xOffset*2));
        glMultMatrixf(glm::value_ptr(model));  
        createStreetWithWorldCoordinates(streetLength*0.675, streetWidth, true, true, 0.985f, 0.941f, streetHeight, model, streetIndex++, worldCoordinates, closestStreetIndex, closestPointPercentage);
        model = glm::mat4(1.0f);
    glPopMatrix();
}
