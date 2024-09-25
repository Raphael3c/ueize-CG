#include <GL/gl.h>
#include "main.h"

void drawInfiniteLines() {
    glColor3f(1.0f, 0.6f, 0.6f); // vermelho claro para parte negativa
    glBegin(GL_LINES);
        glVertex3f(-1000.0f, 0.0f, 0.0f); 
        glVertex3f(0.0f, 0.0f, 0.0f);     
    glEnd();
    
    glColor3f(1.0f, 0.0f, 0.0f); // vermelho escuro para parte positiva
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     
        glVertex3f(1000.0f, 0.0f, 0.0f); 
    glEnd();

    glColor3f(0.6f, 1.0f, 0.6f); // verde claro para parte negativa
    glBegin(GL_LINES);
        glVertex3f(0.0f, -1000.0f, 0.0f); 
        glVertex3f(0.0f, 0.0f, 0.0f);    
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // verde escuro para parte positiva
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     
        glVertex3f(0.0f, 1000.0f, 0.0f);
    glEnd();

    glColor3f(0.6f, 0.6f, 1.0f); 
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -1000.0f); 
        glVertex3f(0.0f, 0.0f, 0.0f); 
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f); 
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     
        glVertex3f(0.0f, 0.0f, 1000.0f);  
    glEnd();
}
