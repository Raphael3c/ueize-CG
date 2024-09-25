#include <GL/gl.h>
#include "main.h"

void drawInfiniteLines() {
    // Eixo X - Parte negativa em vermelho claro, parte positiva em vermelho escuro
    glColor3f(1.0f, 0.6f, 0.6f); // vermelho claro para parte negativa
    glBegin(GL_LINES);
        glVertex3f(-1000.0f, 0.0f, 0.0f); // Parte negativa do eixo X
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
    glEnd();
    
    glColor3f(1.0f, 0.0f, 0.0f); // vermelho escuro para parte positiva
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
        glVertex3f(1000.0f, 0.0f, 0.0f);  // Parte positiva do eixo X
    glEnd();

    // Eixo Y - Parte negativa em verde claro, parte positiva em verde escuro
    glColor3f(0.6f, 1.0f, 0.6f); // verde claro para parte negativa
    glBegin(GL_LINES);
        glVertex3f(0.0f, -1000.0f, 0.0f); // Parte negativa do eixo Y
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // verde escuro para parte positiva
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
        glVertex3f(0.0f, 1000.0f, 0.0f);  // Parte positiva do eixo Y
    glEnd();

    // Eixo Z - Parte negativa em azul claro, parte positiva em azul escuro
    glColor3f(0.6f, 0.6f, 1.0f); // azul claro para parte negativa
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -1000.0f); // Parte negativa do eixo Z
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f); // azul escuro para parte positiva
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);     // Origem
        glVertex3f(0.0f, 0.0f, 1000.0f);  // Parte positiva do eixo Z
    glEnd();
}
