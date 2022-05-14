#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static unsigned int CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    /* Error handling */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* error = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, error);
        std::cout << "Rei err: " << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << error;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int Program = glCreateProgram();
    unsigned int VertexShader = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int FragmentShader = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    /* Linking */
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);
    glValidateProgram(Program);

    /* Cleanup */
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}

int main(void)
{
    GLFWwindow* window;

    std::cout << "Rei" << std::endl;

    if (!glfwInit())
        return -1;

    /* Create a window with OpenGL context */
    window = glfwCreateWindow(720, 720, "Rei Ayanami", NULL, NULL);
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
    unsigned int buffer;
    glGenBuffers(1, &buffer);

    float pos[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    /* Bind and Enable buffer! */
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string VertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}";

    std::string FragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}";

    unsigned int shader = CreateShaders(VertexShader, FragmentShader);
    glUseProgram(shader);

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
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}