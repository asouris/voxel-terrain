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

#include "utils.h"

#define WIDTH  800
#define HEIGHT 600 


int main(){

    /* Controller */
    Controller controller = Controller(WIDTH, HEIGHT);

    /* Window */
    Window window = Window(controller);

    glfwSetFramebufferSizeCallback(window.m_glfwWindow, [](GLFWwindow* window, int w, int h){glViewport(0, 0, w, h);});

    GLuint vs = controller.load_shader("src/shaders/perlin_vertex.glsl", 0);
    GLuint fs = controller.load_shader("src/shaders/perlin_fragment.glsl", 1);

    GLuint program = controller.create_shader_program(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    float voxelSize = 0.1;

    // posición.x, posición.y, posición.z, normal.x, normal.y, normal.z
    float cubeVerts[] = {
        // cara trasera (z = -0.5), normal (0,0,-1)
        -voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,
        voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,
        voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,
        voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,
        -voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,
        -voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f, 0.0f,-1.0f,

        // cara delantera (z = +0.5), normal (0,0,1)
        -voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,
        voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,
        voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,
        voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,
        -voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,
        -voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f, 0.0f, 1.0f,

        // izquierda (x = -0.5), normal (-1,0,0)
        -voxelSize/2, voxelSize/2, voxelSize/2,  -1.0f, 0.0f, 0.0f,
        -voxelSize/2, voxelSize/2,-voxelSize/2,  -1.0f, 0.0f, 0.0f,
        -voxelSize/2,-voxelSize/2,-voxelSize/2,  -1.0f, 0.0f, 0.0f,
        -voxelSize/2,-voxelSize/2,-voxelSize/2,  -1.0f, 0.0f, 0.0f,
        -voxelSize/2,-voxelSize/2, voxelSize/2,  -1.0f, 0.0f, 0.0f,
        -voxelSize/2, voxelSize/2, voxelSize/2,  -1.0f, 0.0f, 0.0f,

        // derecha (x = +0.5), normal (1,0,0)
        voxelSize/2, voxelSize/2, voxelSize/2,   1.0f, 0.0f, 0.0f,
        voxelSize/2, voxelSize/2,-voxelSize/2,   1.0f, 0.0f, 0.0f,
        voxelSize/2,-voxelSize/2,-voxelSize/2,   1.0f, 0.0f, 0.0f,
        voxelSize/2,-voxelSize/2,-voxelSize/2,   1.0f, 0.0f, 0.0f,
        voxelSize/2,-voxelSize/2, voxelSize/2,   1.0f, 0.0f, 0.0f,
        voxelSize/2, voxelSize/2, voxelSize/2,   1.0f, 0.0f, 0.0f,

        // abajo (y = -0.5), normal (0,-1,0)
        -voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f,-1.0f, 0.0f,
        voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f,-1.0f, 0.0f,
        voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f,-1.0f, 0.0f,
        voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f,-1.0f, 0.0f,
        -voxelSize/2,-voxelSize/2, voxelSize/2,   0.0f,-1.0f, 0.0f,
        -voxelSize/2,-voxelSize/2,-voxelSize/2,   0.0f,-1.0f, 0.0f,

        // arriba (y = +0.5), normal (0,1,0)
        -voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 1.0f, 0.0f,
        voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 1.0f, 0.0f,
        voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 1.0f, 0.0f,
        voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 1.0f, 0.0f,
        -voxelSize/2, voxelSize/2, voxelSize/2,   0.0f, 1.0f, 0.0f,
        -voxelSize/2, voxelSize/2,-voxelSize/2,   0.0f, 1.0f, 0.0f
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
    int gridSize = 1500;
    float spacing  = voxelSize;

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
    while(!glfwWindowShouldClose(window.m_glfwWindow)){
        glfwPollEvents();
        controller.camera.update();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &controller.camera.get_camera_view()[0][0]);

        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);

        glfwSwapBuffers(window.m_glfwWindow);
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteProgram(program);
    glfwDestroyWindow(window.m_glfwWindow);
    glfwTerminate();
    return 0;
}
