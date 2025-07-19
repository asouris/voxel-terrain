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
#include <algorithm>

#define WIDTH  800
#define HEIGHT 600 

#define CHUNK_SIZE 32
#define VOXEL_SIZE 0.15

class Chunk{
    public:

    Chunk(size_t gx, size_t gy): 
        globalX(gx),
        globalY(gy)
    {
        float voxelSize = VOXEL_SIZE;

        float cubeVerts[] = {
            // cara trasera (z = -(voxelSize / 2), normal (0,0,-1)
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,
            (voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,
            (voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,
            (voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,
            -(voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f, 0.0f,-1.0f,

            // cara delantera (z = +(voxelSize / 2), normal (0,0,1)
            -(voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,
            (voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,
            -(voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,
            -(voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f, 0.0f, 1.0f,

            // izquierda (x = -(voxelSize / 2), normal (-1,0,0)
            -(voxelSize / 2), (voxelSize / 2), (voxelSize / 2),  -1.0f, 0.0f, 0.0f,
            -(voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),  -1.0f, 0.0f, 0.0f,
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),  -1.0f, 0.0f, 0.0f,
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),  -1.0f, 0.0f, 0.0f,
            -(voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),  -1.0f, 0.0f, 0.0f,
            -(voxelSize / 2), (voxelSize / 2), (voxelSize / 2),  -1.0f, 0.0f, 0.0f,

            // derecha (x = +(voxelSize / 2), normal (1,0,0)
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   1.0f, 0.0f, 0.0f,
            (voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   1.0f, 0.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   1.0f, 0.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   1.0f, 0.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   1.0f, 0.0f, 0.0f,
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   1.0f, 0.0f, 0.0f,

            // abajo (y = -(voxelSize / 2), normal (0,-1,0)
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f,-1.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f,-1.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f,-1.0f, 0.0f,
            (voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f,-1.0f, 0.0f,
            -(voxelSize / 2),-(voxelSize / 2), (voxelSize / 2),   0.0f,-1.0f, 0.0f,
            -(voxelSize / 2),-(voxelSize / 2),-(voxelSize / 2),   0.0f,-1.0f, 0.0f,

            // arriba (y = +(voxelSize / 2), normal (0,1,0)
            -(voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 1.0f, 0.0f,
            (voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 1.0f, 0.0f,
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 1.0f, 0.0f,
            (voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 1.0f, 0.0f,
            -(voxelSize / 2), (voxelSize / 2), (voxelSize / 2),   0.0f, 1.0f, 0.0f,
            -(voxelSize / 2), (voxelSize / 2),-(voxelSize / 2),   0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        offsets.reserve(chunkBlocks);
        for(int x = 0; x < CHUNK_SIZE; x++)
            for(int y = 0; y < CHUNK_SIZE; y++)
                for(int z = 0; z < CHUNK_SIZE / 2; z++)
                offsets.emplace_back(
                    globalX * (CHUNK_SIZE * voxelSize) + x * voxelSize,
                    globalY * (CHUNK_SIZE * voxelSize) + y * voxelSize,
                    int((CHUNK_SIZE * voxelSize) / 4)  - z * voxelSize
                );

        
        glGenBuffers(1, &offsetVBO);
        glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
        glBufferData(GL_ARRAY_BUFFER, chunkBlocks * sizeof(glm::vec3), offsets.data(), GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void draw(){
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunkBlocks);
    }

    private:

    float chunkSize;
    GLuint cubeVAO, cubeVBO, offsetVBO; 
    size_t globalX, globalY;
    std::vector<glm::vec3> offsets;
    size_t chunkBlocks = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 2;

};


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

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"Failed to initialize GLAD\n"; return -1;
    }

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

    GLuint vs = load_shader("src/shaders/perlin-chunks_vertex.glsl", GL_VERTEX_SHADER);
    GLuint fs = load_shader("src/shaders/perlin-chunks_fragment.glsl", GL_FRAGMENT_SHADER);
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

    glm::mat4 projection = glm::perspective(
        glm::radians(40.0f),
        float(WIDTH) / float(HEIGHT),
        0.1f,
        1000.0f
    );

    glm::mat4 view = glm::lookAt(
        glm::vec3(-10.0f, -10.0f, 5.0f), // eye
        glm::vec3(0.0f, 0.0f, 0.0f), // target
        glm::vec3(0.0f, 0.0f, 1.0f)  // up
    );

    glUseProgram(program);
    GLint projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    GLint viewLoc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    GLint voxelSizeLoc = glGetUniformLocation(program, "view");
    glUniform1f(voxelSizeLoc, VOXEL_SIZE);


    size_t size = 12;
    size_t half = size / 2;

    std::vector<Chunk> chunks;
    chunks.reserve(size * size);
    for (size_t i = 0; i < size; i++)
        for(size_t j = 0; j < size; j++){
            if(i == half + 2 && j == half + 2) continue;
            if(i == half + 1 && j == half + 1) continue;
            chunks.emplace_back(Chunk(i - half, j - half));
        }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        
        glClearColor(128/255, 0.0f, 128/255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::for_each(chunks.begin(), chunks.end(), [&](Chunk &c){c.draw();});

        glfwSwapBuffers(window);
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
