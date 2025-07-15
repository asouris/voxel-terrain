#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <math.h>

const int WIDTH = 1000, HEIGHT = 1000;

/* MAIN */

int main()
{

    /* Controller */
    Controller controller = Controller(WIDTH, HEIGHT);

    /* Window */
    Window window = Window(controller);

    /*Load and create shader variables*/
    unsigned int vertex_3d_shader = controller.load_shader("shaders/3d_vertex.glsl", 0);
    unsigned int fragment_3d_shader = controller.load_shader("shaders/3d_fragment.glsl", 1);

    /*Define shader program*/
    unsigned int shader_program_3d = controller.create_shader_program(vertex_3d_shader, fragment_3d_shader);

    /*Delete shaders once they are in a program*/
    glDeleteShader(vertex_3d_shader);
    glDeleteShader(fragment_3d_shader);

    /*Get vertices for the gridlines and positions of all squares*/
    
    /*Primitive for a white cube*/
    float new_cube_vertices[] = {                                                   //normales
        0.0, 0.0, 0.0,                                                              0.0f, -1.0f, 0.0f, // 0
        0.0, 0.0, controller.cell_gl_size,                                          0.0f, -1.0f, 0.0f, // 1
        controller.cell_gl_size, 0.0, 0.0,                                          0.0f, -1.0f, 0.0f, // 4
        controller.cell_gl_size, 0.0, 0.0,                                          0.0f, -1.0f, 0.0f, // 4
        0.0, 0.0, 0.0,                                                              0.0f, -1.0f, 0.0f, // 0
        controller.cell_gl_size, 0.0, controller.cell_gl_size,                      0.0f, -1.0f, 0.0f, // 5

        0.0, controller.cell_gl_size, controller.cell_gl_size,                      0.0f, 1.0f, 0.0f,  // 3
        0.0, controller.cell_gl_size, 0.0,                                          0.0f, 1.0f, 0.0f,  // 2
        controller.cell_gl_size, controller.cell_gl_size, 0.0,                      0.0f, 1.0f, 0.0f,  // 6
        0.0, controller.cell_gl_size, controller.cell_gl_size,                      0.0f, 1.0f, 0.0f,  // 3
        controller.cell_gl_size, controller.cell_gl_size, 0.0,                      0.0f, 1.0f, 0.0f,  // 6
        controller.cell_gl_size, controller.cell_gl_size, controller.cell_gl_size,  0.0f, 1.0f, 0.0f,  // 7

        0.0, controller.cell_gl_size, 0.0,                                          0.0, 0.0, -1.0, // 2
        0.0, 0.0, 0.0,                                                              0.0, 0.0, -1.0, // 0
        controller.cell_gl_size, 0.0, 0.0,                                          0.0, 0.0, -1.0, // 4
        0.0, controller.cell_gl_size, 0.0,                                          0.0, 0.0, -1.0, // 2
        controller.cell_gl_size, 0.0, 0.0,                                          0.0, 0.0, -1.0, // 4
        controller.cell_gl_size, controller.cell_gl_size, 0.0,                      0.0, 0.0, -1.0, // 6

        controller.cell_gl_size, controller.cell_gl_size, 0.0,                      1.0, 0.0, 0.0,  // 6
        controller.cell_gl_size, 0.0, 0.0,                                          1.0, 0.0, 0.0,  // 4
        controller.cell_gl_size, 0.0, controller.cell_gl_size,                      1.0, 0.0, 0.0,  // 5
        controller.cell_gl_size, controller.cell_gl_size, 0.0,                      1.0, 0.0, 0.0,  // 6
        controller.cell_gl_size, 0.0, controller.cell_gl_size,                      1.0, 0.0, 0.0,  // 5
        controller.cell_gl_size, controller.cell_gl_size, controller.cell_gl_size,  1.0, 0.0, 0.0,  // 7

        controller.cell_gl_size, controller.cell_gl_size, controller.cell_gl_size,  0.0, 0.0, 1.0,  // 7
        controller.cell_gl_size, 0.0, controller.cell_gl_size,                      0.0, 0.0, 1.0,  // 5
        0.0, 0.0, controller.cell_gl_size,                                          0.0, 0.0, 1.0,  // 1
        controller.cell_gl_size, controller.cell_gl_size, controller.cell_gl_size,  0.0, 0.0, 1.0,  // 7
        0.0, 0.0, controller.cell_gl_size,                                          0.0, 0.0, 1.0,  // 1
        0.0, controller.cell_gl_size, controller.cell_gl_size,                      0.0, 0.0, 1.0,  // 3

        0.0, controller.cell_gl_size, controller.cell_gl_size,                      -1.0, 0.0, 0.0, // 3
        0.0, 0.0, controller.cell_gl_size,                                          -1.0, 0.0, 0.0, // 1
        0.0, 0.0, 0.0,                                                              -1.0, 0.0, 0.0, // 0
        0.0, 0.0, controller.cell_gl_size,                                          -1.0, 0.0, 0.0, // 1
        0.0, 0.0, 0.0,                                                              -1.0, 0.0, 0.0, // 0
        0.0, controller.cell_gl_size, 0.0,                                          -1.0, 0.0, 0.0  // 2
    };

    

    /*Vertex Buffer Object and Vertex Array Object creation*/
    unsigned int VBOs[3], VAOs[3], EBO;
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);

    /*binding second array: just a quad*/
    controller.bind_load_normals_buffer(VBOs, VAOs, sizeof(new_cube_vertices), new_cube_vertices, 0);

    std::vector <int> voxel_grid(controller.rows * controller.cols * controller.planes, 0); 

    int active_voxels = 40;

    controller.addnRandomVoxels(voxel_grid, active_voxels);

    /*Get vertices for the gridlines and positions of all squares*/
    std::vector<float> points_3d = controller.grid_points_3d();

    std::vector<float> active_voxels_positions = controller.getActiveVoxels(voxel_grid, points_3d, active_voxels);

    /*Binding a special array for instancing the quad, it will hold positions*/
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * active_voxels_positions.size(), active_voxels_positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);	
    glVertexAttribDivisor(2, 1);  

    /*initial view matrices*/
    glm::mat4 projection_3d    = glm::mat4(1.0f);
    glm::mat4 view             = glm::mat4(1.0f);
    glm::mat4 projection       = glm::mat4(1.0f);
    projection_3d = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    
    /*Rendering function*/
    auto render = [&](){

        /*background color*/
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*update some state variables*/
        controller.camera.update();

        /* get uniform locations */
        unsigned int color_loc = glGetUniformLocation(shader_program_3d, "uniColor");
        unsigned int viewLoc  = glGetUniformLocation(shader_program_3d, "view");
        unsigned int projLoc  = glGetUniformLocation(shader_program_3d, "projection");


        glUseProgram(shader_program_3d);

        /* updates uniforms */
        glUniform3fv(color_loc, 1, controller.cell_color);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(controller.camera.get_camera_view()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_3d));
        
        glBindVertexArray(VAOs[0]);
        
        /*Draws cell instances*/
        glDrawArraysInstanced(GL_TRIANGLES, 0, 30, 100); 
        
        /*draws Imgui interface*/
        controller.renderImgui(window.m_glfwWindow, (*window.io));
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glBindVertexArray(0);
        glfwSwapBuffers(window.m_glfwWindow);
    };


    /*Window loop, controls the speed for the simulation*/
    float last_time = glfwGetTime();
    float last_update_time = glfwGetTime();
    float accumulated_time = 0.f;


    while (!glfwWindowShouldClose(window.m_glfwWindow))
    {
        float current_time = glfwGetTime();
        float dt = current_time - last_time;
        last_time = current_time;
        accumulated_time += dt;

        glfwPollEvents();   
        

        /*rendering*/
        float FRAME_TIME = 1.f / (float)controller.current_fps;
        if (accumulated_time >= FRAME_TIME)
        {
            float update_dt = current_time - last_update_time;
            last_update_time = current_time;
            accumulated_time = 0.f;

            render();
        }
    
    }

    /*if the window was closed, frees memory*/
    glDeleteVertexArrays(4, VAOs);
    glDeleteBuffers(4, VBOs);

    glfwTerminate();
    return 0;   
  
}
