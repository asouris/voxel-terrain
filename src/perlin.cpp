// main.cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <unistd.h>
#include <vector>

#define WIDTH  800
#define HEIGHT 600 

void framebuffer_size_callback(GLFWwindow* window, int w, int h){
    glViewport(0, 0, w, h);
}

GLuint load_shader(const std::string& path, GLenum shaderType) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: couldn't load shader in path: " << path << "\n";
        return 0;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string srcStr = buffer.str();
    const char* src = srcStr.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Error while compiling shader (" << path << "):\n" 
                  << log << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test", nullptr, nullptr);
    if(!window){ std::cerr<<"Error creating window\n"; return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    //     std::cerr<<"Failed to initialize GLAD\n"; return -1;
    // }

    auto compile = [&](GLenum type, const char* src){
        GLuint s = glCreateShader(type);
        glShaderSource(s,1,&src,nullptr);
        glCompileShader(s);
        GLint ok;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if(!ok){
            char buf[512]; glGetShaderInfoLog(s,512,nullptr,buf);
            std::cerr<<"Shader compile error: "<<buf<<"\n";
        }
        return s;
    };

    GLuint vs = load_shader("src/shaders/perlin_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = load_shader("src/shaders/perlin_fragment.glsl", GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    

    GLint linkOk;
    glGetProgramiv(program, GL_LINK_STATUS, &linkOk);
    if (!linkOk) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // posición.x, posición.y, posición.z, normal.x, normal.y, normal.z
    float cubeVerts[] = {
        // cara trasera (z = -0.5), normal (0,0,-1)
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
        0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
        0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
        0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
        -0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,

        // cara delantera (z = +0.5), normal (0,0,1)
        -0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,

        // izquierda (x = -0.5), normal (-1,0,0)
        -0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,-0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,-0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,-0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,

        // derecha (x = +0.5), normal (1,0,0)
        0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
        0.5f, 0.5f,-0.5f,   1.0f, 0.0f, 0.0f,
        0.5f,-0.5f,-0.5f,   1.0f, 0.0f, 0.0f,
        0.5f,-0.5f,-0.5f,   1.0f, 0.0f, 0.0f,
        0.5f,-0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,

        // abajo (y = -0.5), normal (0,-1,0)
        -0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,
        0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,
        0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,
        0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,
        -0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,
        -0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,

        // arriba (y = +0.5), normal (0,1,0)
        -0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,
        0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f
    };

    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    std::vector<glm::vec2> offsets;
    int gridSize = 750;
    float spacing  = 1.0f;

    offsets.clear();
    offsets.reserve(gridSize * gridSize);

    float half = (gridSize - 1) * spacing * 0.5f;

    for(int ix = 0; ix < gridSize; ++ix) {
        for(int iy = 0; iy < gridSize; ++iy) {
            float x = ix * spacing - half;
            float y = iy * spacing - half;
            offsets.emplace_back(x, y);
        }
    }

    int amount = gridSize * gridSize;

    GLuint offsetVBO;
    glGenBuffers(1, &offsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::vec2), offsets.data(), GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(WIDTH) / float(HEIGHT),
        0.1f,
        1000.0f
    );

    glm::mat4 view = glm::lookAt(
        glm::vec3(25.0f, 25.0f, 20.0f), // eye
        glm::vec3(0.0f, 0.0f, 0.0f), // target
        glm::vec3(0.0f, 0.0f, 1.0f)  // up
    );

    glUseProgram(program);
    GLint projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    GLint viewLoc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);


    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        glUseProgram(program);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
