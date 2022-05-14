#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    std::cout << "Rei" << std::endl;

    if (!glfwInit())
        return -1;

    /* Create a window with OpenGL context */
    window = glfwCreateWindow(640, 480, "Rei Ayanami", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Rei dead" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    /* Exit */
    glfwTerminate();
    return 0;
}