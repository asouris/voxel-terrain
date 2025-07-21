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
GLuint chunksProgram;
GLuint cullingProgram;

class Chunk{
    public:

    Chunk(long gx, long gz): 
        initialGlobalX(gx),
        initialGlobalZ(gz)
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
        glBufferData(GL_SHADER_STORAGE_BUFFER, chunkBlocks * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsetSSBO);

        glBindBuffer(GL_ARRAY_BUFFER, offsetSSBO);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glGenBuffers(1, &offsetCounterBuff);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, offsetCounterBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &drawCounterBuff);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, drawCounterBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
        
        glGenBuffers(1, &drawSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, chunkBlocks * sizeof(int), nullptr, GL_DYNAMIC_DRAW);

    }

    void draw(){
        globalX = initialGlobalX + cameraChunk[0];
        globalZ = initialGlobalZ + cameraChunk[1];

        glUseProgram(chunksProgram);
        glUniform1i(glGetUniformLocation(chunksProgram, "globalX"), globalX);
        glUniform1i(glGetUniformLocation(chunksProgram, "globalZ"), globalZ);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, drawSSBO);

        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, drawCounterBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, drawCounterBuff);

        glDispatchCompute(CHUNK_SIZE/8, CHUNK_SIZE*2/8, CHUNK_SIZE/8);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, drawCounterBuff);
        GLuint *cnt = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

        // std::cout << "draws" <<*cnt << std::endl;
        if (*cnt == 0) return;

        glUseProgram(cullingProgram);
        glUniform1i(glGetUniformLocation(cullingProgram, "globalX"), globalX);
        glUniform1i(glGetUniformLocation(cullingProgram, "globalZ"), globalZ);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, offsetSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, drawSSBO);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, offsetCounterBuff);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, offsetCounterBuff);

        glDispatchCompute(CHUNK_SIZE/8, CHUNK_SIZE*2/8, CHUNK_SIZE/8);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, offsetCounterBuff);
        cnt = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT);
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
        
        // std::cout << "instances" <<*cnt << std::endl;
        if (*cnt == 0) return;

        glUseProgram(program);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, offsetSSBO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, *cnt);

    }

    void update(long cameraX, long cameraZ){
        cameraChunk[0] = cameraX;
        cameraChunk[1] = cameraZ;
    }

    private:

    float chunkSize;
    GLuint cubeVAO, cubeVBO, offsetSSBO, drawSSBO, offsetCounterBuff, drawCounterBuff, zero = 0; 
    long initialGlobalX, initialGlobalZ, globalX, globalZ;
    uint chunkBlocks = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 2;
    long cameraChunk[2] = {0, 0};

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

    chunksProgram = controller.get_compute_program("src/shaders/chunks_compute.glsl");


    glUseProgram(chunksProgram);
    glUniform1f(glGetUniformLocation(chunksProgram, "voxelSize"), VOXEL_SIZE);
    glUniform1i(glGetUniformLocation(chunksProgram, "chunkSize"), CHUNK_SIZE);

    cullingProgram = controller.get_compute_program("src/shaders/culling_compute.glsl");
    glUseProgram(cullingProgram);
    glUniform1f(glGetUniformLocation(cullingProgram, "voxelSize"), VOXEL_SIZE);
    glUniform1i(glGetUniformLocation(cullingProgram, "chunkSize"), CHUNK_SIZE);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(WIDTH) / float(HEIGHT),
        0.1f,
        1000.0f
    );

    glUseProgram(program);
    GLint projLoc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    long size = 16;
    long half = size / 2;

    std::vector<Chunk> chunks;
    chunks.reserve(size * size);
    for (long i = 0; i < size; i++)
        for(long j = 0; j < size; j++){
            chunks.emplace_back(Chunk(i - half, j - half));
        }

    controller.camera.render = 2;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    while(!glfwWindowShouldClose(window.m_glfwWindow)){
        glfwPollEvents();
        
        glClearColor(128/255, 0.0f, 128/255, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        controller.camera.update();
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &controller.camera.get_camera_view()[0][0]);

        glUseProgram(chunksProgram);
        glUniform1i(glGetUniformLocation(chunksProgram, "renderDistance"), controller.camera.render);
        float cameraPos[2] = {controller.camera.offset.x, controller.camera.offset.z};
        glUniform2fv(glGetUniformLocation(chunksProgram, "cameraPos"), 1, cameraPos);

        std::for_each(chunks.begin(), chunks.end(), 
            [&](Chunk &c)
            {
                c.update(
                    floor(controller.camera.offset.x / (CHUNK_SIZE * VOXEL_SIZE)),
                    floor(controller.camera.offset.z / (CHUNK_SIZE * VOXEL_SIZE))
                );

                c.draw();
            });
        
        // exit(2);


        glfwSwapBuffers(window.m_glfwWindow);
    }

    glDeleteProgram(program);
    glfwDestroyWindow(window.m_glfwWindow);
    glfwTerminate();
    return 0;
}