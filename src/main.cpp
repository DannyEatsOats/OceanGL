#include <cstdint>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <glad/glad.h>
#include <sstream>
#include <vector>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
    //=== Triangle ===
    float vertices[] = {
        -0.75f, -0.5f, 0.0f, // First Bottom Left
        -0.25f, -0.5f, 0.0f, // First Bottom Right
        -0.5f,  0.5f,  0.0f, // First Bottom Top

        0.25f,  -0.5f, 0.0f, // First Bottom Right
        0.75f,  -0.5f, 0.0f, // First Bottom Left
        0.5f,   0.5f,  0.0f, // First Bottom Top
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        3, 4, 5  // seconds triangle
    };
*/

struct Window {
    struct WindowData {
        uint32_t Width;
        uint32_t Height;
    };

    GLFWwindow *WindowPtr = nullptr;
    WindowData Data = {0, 0};

    Window(WindowData data) : Data(data) {
        Data = {1280, 720};

        // Init GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


        // Create Window
        WindowPtr = glfwCreateWindow(1280, 720, "Window Name", nullptr, nullptr);

        if (WindowPtr == nullptr) {
            glfwTerminate();
            throw std::runtime_error("Failed to initualize GLFW window");
        }

        glfwMakeContextCurrent(WindowPtr);
        glfwSetWindowUserPointer(WindowPtr, &Data);
        glfwSetFramebufferSizeCallback(WindowPtr, framebuffer_size_callback);

        // Load GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

        glViewport(0, 0, (GLsizei)1280, (GLsizei)720);
    }

    void Clear() {
        // CLearing
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Update() {
        glfwSwapBuffers(WindowPtr);
        glfwPollEvents();
    }

    void ProcessInput() {
        if (glfwGetKey(WindowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(WindowPtr, true);
    }

    bool ShouldClose() { return glfwWindowShouldClose(WindowPtr); }

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);

        auto *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        if (data) {
            data->Width = width;
            data->Height = height;
        }
    }
};

struct VertexArray {
    GLuint RendererID;

    VertexArray() { glGenVertexArrays(1, &RendererID); }

    void Bind() const { glBindVertexArray(RendererID); }
    void UnBind() const { glBindVertexArray(0); }
    void Delete() const {glDeleteVertexArrays(1, &RendererID);}

    void AttachAttribPtr(GLuint location, GLuint size, GLuint stride, const void *offset) const {
        glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(location);
    }
};

struct VertexBuffer {
    GLuint RendererID;

    VertexBuffer() { glGenBuffers(1, &RendererID); }

    void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, RendererID); }
    void UnBind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    void Delete() const {glDeleteBuffers(1, &RendererID);}

    void BufferData(const void *vertices, GLuint size) const {
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }
};

struct ElementBuffer {
    GLuint RendererID;

    ElementBuffer() { glGenBuffers(1, &RendererID); }

    void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID); }
    void UnBind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    void Delete() const  { glDeleteBuffers(1, &RendererID); }

    void BufferData(const void *elements, GLuint size) const {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, elements, GL_STATIC_DRAW);
    }
};

struct Shader {
    GLuint RendererID;

    Shader(const char *vertexPath, const char *fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;

        std::cout << vertexPath << std::endl;
        std::cout << fragmentPath << std::endl;

        try {
            std::ifstream vShaderFile(vertexPath, std::ios::in);
            std::ifstream fShaderFile(fragmentPath, std::ios::in);

            if (!vShaderFile.is_open())
                throw std::runtime_error("Failed to open vertex shader: " +
                                         std::string(vertexPath));

            if (!fShaderFile.is_open())
                throw std::runtime_error("Failed to open fragment shader: " +
                                         std::string(fragmentPath));

            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (const std::exception &e) {
            std::cerr << "ERROR::SHADER::FILE_COULD_NOT_BE_READ: " << e.what() << std::endl;
            throw; // stop execution
        }
        std::cout << "Vertex shader path: " << vertexPath << std::endl;
        std::cout << "Fragment shader path: " << fragmentPath << std::endl;
        std::cout << "Vertex shader size: " << vertexCode.size() << " bytes" << std::endl;
        std::cout << "Fragment shader size: " << fragmentCode.size() << " bytes" << std::endl;

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        GLuint vertex, fragment;
        int success;
        char info[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, info);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info << std::endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, info);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info << std::endl;
        }

        RendererID = glCreateProgram();
        glAttachShader(RendererID, vertex);
        glAttachShader(RendererID, fragment);
        glLinkProgram(RendererID);

        glGetProgramiv(RendererID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(RendererID, 512, NULL, info);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Activate() const { glUseProgram(RendererID); }

    void Delete() const { glDeleteProgram(RendererID); }

    void UploadBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(RendererID, name.c_str()), (int)value);
    }

    void UploadInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(RendererID, name.c_str()), value);
    }

    void UploadFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(RendererID, name.c_str()), value);
    }

    void UploadFloat3(const std::string &name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(RendererID, name.c_str()), 1, glm::value_ptr(value));
    }

    void UploadMat4(const std::string& name, const glm::mat4& matrix) const {
        glUniformMatrix4fv(glGetUniformLocation(RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }
};

class Plane {
public:
    Plane(float width, float height, unsigned int xSegment, unsigned int zSegment) {
        Generate(width, height, xSegment, zSegment);
    }

    const std::vector<float>& GetVectices() const {return m_Vertices;}
    const std::vector<unsigned int>& GetIndices() const {return m_Indices;}

private:
    void Generate(float width, float height, unsigned int xSegment, unsigned int zSegment) {
        m_Vertices.clear();
        m_Indices.clear();

        for(unsigned int i = 0; i <= zSegment; ++i) {
            float z = ((float)i / zSegment - 0.5f) * height;
            for (unsigned int j = 0; j<=xSegment; ++j) {
                float x = ((float)j / xSegment - 0.5f) * width;
                float y = 0.0f;
                m_Vertices.push_back(x);
                m_Vertices.push_back(y);
                m_Vertices.push_back(z);
            }
        }
    
        for(unsigned int i = 0; i < zSegment; ++i) {
            for (unsigned int j = 0; j< xSegment; ++j) {
                unsigned int row1 = i * (xSegment + 1);
                unsigned int row2 = (i + 1) * (xSegment + 1);

                m_Indices.push_back(row1 + j);
                m_Indices.push_back(row2 + j);
                m_Indices.push_back(row1 + j + 1);

                m_Indices.push_back(row1 + j + 1);
                m_Indices.push_back(row2 + j);
                m_Indices.push_back(row2 + j + 1);
            }
        }
        
    }

    std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Indices;

};

int main(int argc, char *argv[]) {
    // Data
    Window window({1280, 720});

    Plane ocean(10.0f, 10.0f, 3000, 3000);
    glm::vec3 camera = glm::vec3(0.0f, 0.1f, 12.0f);

    // Generate VAO, VBO, EBO order matters
    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;

    // 1. Vind Vertex Array, it records state, Just have to bind the VAO before drawing
    VAO.Bind();

    // 2. Bind the Vertex Buffer, and buffer data
    VBO.Bind();
    VBO.BufferData(ocean.GetVectices().data(), ocean.GetVectices().size() * sizeof(float));

    // 3. Bind the Element Buffer, and buffer indices
    EBO.Bind();
    EBO.BufferData(ocean.GetIndices().data(), ocean.GetIndices().size() * sizeof(unsigned int));

    // 4. Configure and enalbe Attrib Pointer
    VAO.AttachAttribPtr(0, 3, sizeof(float) * 3, (void *)0);

    // 5. Unbinding
    VAO.UnBind();
    VBO.UnBind();

    //=== Shaders ===
    Shader shader("assets/shaders/triangle.vert", "assets/shaders/triangle.frag");

    //=== Math ===
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, camera * -1.0f);
    view = glm::rotate(view, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //view = glm::rotate(view, glm::radians(80.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)window.Data.Width / (float)window.Data.Height, 0.1f, 100.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // RENDER LOOP//
    while (!window.ShouldClose()) {
        window.Clear();

        shader.Activate();
        VAO.Bind();

        shader.UploadFloat("time", glfwGetTime());
        shader.UploadMat4("model", model);
        shader.UploadMat4("view", view);
        shader.UploadMat4("projection", projection); 
        shader.UploadFloat3("camPos", camera); 

        glDrawElements(GL_TRIANGLES, ocean.GetIndices().size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        window.ProcessInput();
        window.Update();
    }

    shader.Delete();

    EBO.Delete();
    VBO.Delete();
    VAO.Delete();

    glfwTerminate();
    return 0;
}
