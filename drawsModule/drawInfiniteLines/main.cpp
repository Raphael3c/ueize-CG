// drawsModule/drawInfiniteLines/main.cpp

#include <GL/gl.h>
#include "main.h"

void drawInfiniteLines() {
    glColor3f(1.0f, 0.0f, 0.0f); //red x
    glBegin(GL_LINES);
        glVertex3f(-1000.0f, 0.0f, 0.0f);
        glVertex3f(1000.0f, 0.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // green y
    glBegin(GL_LINES);
        glVertex3f(0.0f, -1000.0f, 0.0f);
        glVertex3f(0.0f, 1000.0f, 0.0f);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f); // blue z
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -1000.0f);
        glVertex3f(0.0f, 0.0f, 1000.0f);
    glEnd();
}
