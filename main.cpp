#include <GLFW/glfw3.h>

double mouseX = 0.0;
double mouseY = 0.0;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        double triangleX = (mouseX / 640.0) * 2.0 - 1.0;
        double triangleY = -((mouseY / 480.0) * 2.0 - 1.0);

        glColor3f(1, 0, 0);
        glBegin(GL_TRIANGLES);
        glVertex2f(triangleX - 0.1, triangleY - 0.1);
        glVertex2f(triangleX + 0.1, triangleY - 0.1);
        glVertex2f(triangleX, triangleY + 0.1);
        glEnd();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
