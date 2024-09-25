#include "main.h"
#include <GL/gl.h>

extern std::vector<glm::vec3> lineCoordinates;

void drawLine() {
    if (lineCoordinates.empty()) return;

    glColor3f(1.0f, 0.0f, 1.0f); 
    glBegin(GL_LINE_STRIP);
    for (const auto& point : lineCoordinates) {
        glVertex3f(point.x, point.y, point.z); 
    }
    glEnd();
}
