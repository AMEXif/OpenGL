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

    /* Vertex buffer */
    float pos[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);

    /* Bind and Enable buffer! */
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    /* Main loop */
    while (!glfwWindowShouldClose(window))
    {
        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap back and front buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    /* Exit */
    glfwTerminate();
    return 0;
}