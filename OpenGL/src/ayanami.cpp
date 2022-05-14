#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderSource ParseShader(const std::string& filepath)
{
    std::ifstream file(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 
    };

    std::string line;
    std::stringstream SourceCode[3];
    ShaderType type = ShaderType::NONE;

    /* Read from shader file */
    while (getline(file, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            /* Set vertex mode */
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;

            /* Set fragment mode */
            else if (line.find("fragment") != std::string::npos) 
                type = ShaderType::FRAGMENT;
        }
        else
        {
            if (type != ShaderType::NONE)
                SourceCode[(int)type] << line << '\n';
            else
                SourceCode[0] << "Dead" << '\n';
        }
    }

    return { SourceCode[0].str(), SourceCode[1].str() };
}

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

    ShaderSource source = ParseShader("resources/shaders/Basic.shader");
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShaders(source.VertexSource, source.FragmentSource);
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