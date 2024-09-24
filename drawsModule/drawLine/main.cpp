#include "main.h"
#include <GL/gl.h>

// Declaração global de lineCoordinates
extern std::vector<glm::vec3> lineCoordinates;

void drawLine() {
    if (lineCoordinates.empty()) return;  // Não há linha para desenhar

    glColor3f(1.0f, 0.0f, 1.0f);  // Definir a cor da linha
    glBegin(GL_LINE_STRIP);
    for (const auto& point : lineCoordinates) {
        glVertex3f(point.x, point.y, point.z);  // Definir os vértices da linha
    }
    glEnd();
}
