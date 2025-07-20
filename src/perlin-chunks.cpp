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

#include "utils.h"

#define WIDTH  800
#define HEIGHT 600 

int   CHUNK_SIZE;
float VOXEL_SIZE;

GLuint program;
GLuint computeProgram;

class Chunk{
    public:

    Chunk(long gx, long gy): 
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

        glGenBuffers(1, &offsetSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetSSBO);
        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            chunkBlocks * 3 * sizeof(float),
            nullptr,
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsetSSBO);

        glBindBuffer(GL_ARRAY_BUFFER, offsetSSBO);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glGenBuffers(1, &counterBuf);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuf);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
        
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, counterBuf);
    }

    void draw(){
        glUseProgram(computeProgram);
        glUniform1i(glGetUniformLocation(computeProgram, "globalX"), globalX);
        glUniform1i(glGetUniformLocation(computeProgram, "globalY"), globalY);
        
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, counterBuf);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuf);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsetSSBO);
        glDispatchCompute(CHUNK_SIZE/8, CHUNK_SIZE/8, CHUNK_SIZE/8);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuf);
        GLuint *cnt = (GLuint*)glMapBufferRange(
                        GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint),
                        GL_MAP_READ_BIT);
        GLuint instanceCount = *cnt;
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
        glUseProgram(program);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, offsetSSBO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, *cnt);

        // {
        //     // bind the SSBO
        //     glBindBuffer(GL_SHADER_STORAGE_BUFFER, offsetSSBO);

        //     // map it for reading
        //     float* ptr = (float*)glMapBufferRange(
        //         GL_SHADER_STORAGE_BUFFER,
        //         0,
        //         chunkBlocks * sizeof(float) * 3,
        //         GL_MAP_READ_BIT
        //     );

        //     if (ptr) {
        //         for (size_t i = 0; i < chunkBlocks; ++i) {
        //             float x = ptr[i*3 + 0], y = ptr[i*3 + 1], z = ptr[i*3 + 2];
        //             std::cout << "SSBO["<<i<<"] = ("<<x<<","<<y<<","<<z<<")\n";
        //         }
        //     }
        // }

    }

    private:

    float chunkSize;
    GLuint cubeVAO, cubeVBO, offsetSSBO, counterBuf, zero = 0; 
    long globalX, globalY;
    std::vector<float> rawOffsets;
    uint chunkBlocks = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

};


int main(int argc, char* argv[]){
    if (argc != 3){
        std::cerr << "Usage: ./" << argv[0] << " <chunk-size> <voxel-size>";
        exit(1);
    }

    CHUNK_SIZE = atoi(argv[1]);
    VOXEL_SIZE = atof(argv[2]);

    /* Controller */
    Controller controller = Controller(WIDTH, HEIGHT);

    /* Window */
    Window window = Window(controller);

    glfwSetFramebufferSizeCallback(window.m_glfwWindow, [](GLFWwindow* window, int w, int h){glViewport(0, 0, w, h);});

    /*Load and create shader variables*/
    GLuint vs = controller.load_shader("src/shaders/chunks_vertex.glsl", 0);
    GLuint fs = controller.load_shader("src/shaders/chunks_fragment.glsl", 1);

    /*Define shader program*/
    program = controller.create_shader_program(vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    /*Define compute program*/
    computeProgram = controller.get_compute_program("src/shaders/chunks_compute.glsl");

    glUseProgram(computeProgram);
    glUniform1f(glGetUniformLocation(computeProgram, "voxelSize"), VOXEL_SIZE);
    glUniform1i(glGetUniformLocation(computeProgram, "chunkSize"), CHUNK_SIZE);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(WIDTH) / float(HEIGHT),
        0.1f,
        1000.0f
    );

    glm::mat4 view = glm::lookAt(
        glm::vec3(7.0f, 7.0f, 5.0f), // eye
        glm::vec3(0.0f, 0.0f, 0.0f), // target
        glm::vec3(0.0f, 0.0f, 1.0f)  // up
    );

    glUseProgram(program);
    GLint projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    GLint viewLoc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    long size = 20;
    long half = size / 2;

    std::vector<Chunk> chunks;
    chunks.reserve(size * size);
    for (long i = 0; i < size; i++)
        for(long j = 0; j < size; j++){
            chunks.emplace_back(Chunk(i - half, j - half));
        }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    while(!glfwWindowShouldClose(window.m_glfwWindow)){
        glfwPollEvents();
        
        glClearColor(128/255, 0.0f, 128/255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::for_each(chunks.begin(), chunks.end(), [&](Chunk &c){c.draw();});

        glfwSwapBuffers(window.m_glfwWindow);
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window.m_glfwWindow);
    glfwTerminate();
    return 0;
}