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

    int n = controller.rows;

    /* Window */
    Window window = Window(controller);

    /*Load and create shader variables*/
    unsigned int vertex_3d_shader = controller.load_shader("shaders/3d_vertex.glsl", 0);
    unsigned int fragment_3d_shader = controller.load_shader("shaders/3d_fragment.glsl", 1);

    /*Define shader program*/
    unsigned int shader_program_3d = controller.create_shader_program(vertex_3d_shader, fragment_3d_shader);

    /*Define compute program*/
    unsigned int compute = controller.get_compute_program("shaders/perlin.glsl");

    /*Delete shaders once they are in a program*/
    glDeleteShader(vertex_3d_shader);
    glDeleteShader(fragment_3d_shader);
    
    /*Primitive for a white cube*/
    std::vector<float> cube_vertices = getCubeVertices(controller.cell_gl_size);

    /*Vertex Buffer Object and Vertex Array Object creation*/
    unsigned int VBOs[3], VAOs[3], EBO;
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);

    /*binding second array: just a quad*/
    controller.bind_load_normals_buffer(VBOs, VAOs, sizeof(float)*cube_vertices.size(), cube_vertices.data(), 0);
    

    /*Texture to save result from compute shader*/
    const unsigned int TEXTURE_WIDTH = controller.rows, TEXTURE_HEIGHT = controller.cols;
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, 
             GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    /*Run compute shader*/
    glUseProgram(compute);
    /*Here you set the amount of workgroups, one per pixel / grid row*/
    glDispatchCompute(TEXTURE_WIDTH, TEXTURE_HEIGHT, 1);
    /*Wait for all threads to finish*/
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    /*Read result from texture into data array*/
    glBindTexture(GL_TEXTURE_2D, texture);
    float* data = new float[TEXTURE_WIDTH * TEXTURE_HEIGHT * 4];
    glGetTextureImage(texture, 0, GL_RGBA, GL_FLOAT, TEXTURE_WIDTH * TEXTURE_HEIGHT * 4 * 4, data);

    /*Get positions of all cubes of grid*/
    std::vector<float> points_3d = controller.grid_points_3d();

    /*We make the voxel grid*/
    std::vector <int> voxel_grid(controller.rows * controller.cols * controller.planes, 0); 

    /*Set initial voxels*/
    int active_voxels = controller.rows * controller.cols;
    for(int i = 0 ; i < controller.rows; i++){
        for(int j = 0; j < controller.planes; j++){
            voxel_grid[worldIdx(i, controller.rows-1, j, controller.rows, controller.rows, controller.rows)] = 1;
        }
    }

    /*Set extra voxels according to height value from noise*/
    for(int i = 0 ; i < controller.rows; i++){
        for(int j = 0; j < controller.cols; j++){
            for(int k = 0; k < controller.planes; k++){
                int voxelID = worldIdx(i, j, k, controller.rows, controller.rows, controller.rows);
                float columnHeight = data[worldIdx(i, k, 0, controller.rows, controller.rows, 1)*4];
                
                float vx = points_3d[voxelID*3], vy = points_3d[voxelID*3+1], vz = points_3d[voxelID*3+2];
                if(vy <= columnHeight){
                    voxel_grid[voxelID] = 1;
                    active_voxels += 1;
                }
            }
        }
    }

    /*Get active voxels positions*/
    std::vector<float> active_voxels_positions = controller.getActiveVoxels(voxel_grid, points_3d, active_voxels);
    
    /*Binding a special array for instancing the cube, it will hold active voxel positions*/
    /*You can find it as aOffset in the shader*/
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
    glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 10.0f);

    /*Rendering function*/
    auto render = [&](){

        /*background color*/
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*update some state variables*/
        controller.camera.update();

        /* get uniform locations */
        unsigned int color_loc = glGetUniformLocation(shader_program_3d, "uniColor");
        unsigned int lightPosLoc = glGetUniformLocation(shader_program_3d, "lightPos");
        unsigned int intensityLoc = glGetUniformLocation(shader_program_3d, "light_intensity");
        unsigned int viewLoc  = glGetUniformLocation(shader_program_3d, "view");
        unsigned int projLoc  = glGetUniformLocation(shader_program_3d, "projection");


        glUseProgram(shader_program_3d);

        /*To read texture*/
        glBindTextureUnit(0, texture);
        glUniform1i(glGetUniformLocation(shader_program_3d, "screen"), 0);


        /* updates uniforms */
        glUniform3fv(color_loc, 1, controller.cell_color);
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform1f(intensityLoc, 1.0);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(controller.camera.get_camera_view()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_3d));
        
        glBindVertexArray(VAOs[0]);
        
        /*Draws cell instances*/
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, active_voxels); 
        
        /*draws Imgui interface*/
        // controller.renderImgui(window.m_glfwWindow, (*window.io));
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

   
    glfwTerminate();
    return 0;   
  
}
