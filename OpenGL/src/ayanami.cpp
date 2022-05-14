#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* Global error handling */
#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) glClearError();\
    x;\
    ASSERT(glLogCall(#x, __FILE__, __LINE__))

static void glClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool glLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[Rei Error]:[OpenGL] ( " << error << " ) " << function << " on line " << line << std::endl;
        return false;
    }
    return true;
}

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
        std::cout << "[Rei Error]:[Compilation] " << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
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

    /* std::cout << "Rei" << std::endl; */

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
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Rei dead" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl << std::endl;

    /* Vertex buffer */
    float pos[] = {
        -0.5f, -0.5f,
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f,  0.5f
    };

    /* Vertex indexing */
    unsigned int indices[] = {
        0, 2, 1,
        1, 3, 0
    };

    /* Bind and Enable buffer! */
    unsigned int buffer;
    glCall(glGenBuffers(1, &buffer));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    glCall(glEnableVertexAttribArray(0));

    glCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), pos, GL_STATIC_DRAW));
    glCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    unsigned int IndexBuffer; 
    glCall(glGenBuffers(1, &IndexBuffer));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderSource source = ParseShader("resources/shaders/Basic.shader");

    /* Manual shader code check */
    /* std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl; */

    unsigned int shader = CreateShaders(source.VertexSource, source.FragmentSource);
    glCall(glUseProgram(shader));

    /* Change color */
    glCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    glCall(glUniform4f(location, 0.18f, 0.53f, 0.9f, 1.0f));

    float r = 0.0f;
    float increment = 0.05f;

    /* Main loop */
    while (!glfwWindowShouldClose(window))
    {
        /* Rendering */
        glClear(GL_COLOR_BUFFER_BIT);

        glCall(glUniform4f(location, r, 0.53f, 0.9f, 1.0f));
        glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)
            increment = -0.05f;
        if (r < 0.0f)
            increment = 0.05f;

        r += increment;

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
