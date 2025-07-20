#include "utils.h"
#include "marching_cubes.h"

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

    glEnable(GL_DEPTH_TEST);

    /*Load and create shader variables*/
    unsigned int vertex_3d_shader = controller.load_shader("shaders/marching_shadows_vertex.glsl", 0);
    unsigned int fragment_3d_shader = controller.load_shader("shaders/marching_shadows_fragment.glsl", 1);

    unsigned int vertex_depth = controller.load_shader("shaders/depth_vertex.glsl", 0);
    unsigned int fragment_depth = controller.load_shader("shaders/depth_fragment.glsl", 1);

    /*Define shader program*/
    unsigned int shader_program_3d = controller.create_shader_program(vertex_3d_shader, fragment_3d_shader);

    /*Define deph program*/
    unsigned int depth_program = controller.create_shader_program(vertex_depth, fragment_depth);

    /*Define compute program*/
    unsigned int compute = controller.get_compute_program("shaders/perlin_3d.glsl");

    /*Delete shaders once they are in a program*/
    glDeleteShader(vertex_3d_shader);
    glDeleteShader(fragment_3d_shader);


    /*Texture to save result from compute shader*/
    const unsigned int TEXTURE_WIDTH = controller.rows, TEXTURE_HEIGHT = controller.cols;
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_HEIGHT, 0, GL_RGBA, 
             GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    /*Run compute shader*/
    glUseProgram(compute);
    /*Here you set the amount of workgroups, one per pixel / grid row*/
    glDispatchCompute(TEXTURE_WIDTH/10, TEXTURE_HEIGHT/10, TEXTURE_HEIGHT/10);
    /*Wait for all threads to finish*/
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    /*Read result from texture into data array*/
    glBindTexture(GL_TEXTURE_3D, texture);
    float* data = new float[TEXTURE_WIDTH * TEXTURE_HEIGHT * TEXTURE_HEIGHT * 4];
    glGetTextureImage(texture, 0, GL_RGBA, GL_FLOAT, TEXTURE_WIDTH * TEXTURE_HEIGHT * TEXTURE_HEIGHT * 4 * 4, data);

    std::cout << "finished getting noise\n";

    /*Get positions of all voxels of grid*/
    std::vector<float> points_3d = controller.grid_points_3d();

    /*The voxel grid, only holds state (1 or 0)*/
    std::vector <int> voxel_grid(controller.rows * controller.cols * controller.planes, 0); 

    /*Set initial voxels, a plane*/
    int active_voxels = controller.rows * controller.cols;
    for(int i = 0 ; i < controller.rows; i++){
        for(int j = 0; j < controller.planes; j++){
            voxel_grid[worldIdx(i, controller.rows-1, j, controller.rows, controller.rows, controller.rows)] = 1;
        }
    }

    /*Set extra voxels according to height value from noise, fill columns until height*/
    for(int i = 0 ; i < controller.rows; i++){
        for(int j = 0; j < controller.rows; j++){
            for(int k = 0; k < controller.rows; k++){
                int voxelID = worldIdx(i, j, k, controller.rows, controller.rows, controller.rows);

                float density = data[voxelID*4];
                if(density > 0.3){
                    voxel_grid[voxelID] = 1;
                    active_voxels += 1;
                }
            }
        }
    }

    /*Positions of the unit cube*/
    float unit = controller.cell_gl_size;
    std::vector<glm::vec3> cubeCorners = {

        glm::vec3(0, unit, unit),
        glm::vec3(unit, unit, unit),
        glm::vec3(unit, unit, 0),
        glm::vec3(0, unit, 0),
        glm::vec3(0, 0, unit),
        glm::vec3(unit, 0, unit),
        glm::vec3(unit, 0, 0),
        glm::vec3(0, 0, 0)
    };

    /*Gets the middle point between the 2 ends of an edge*/
    auto getEdgeMidPoint = [cubeCorners](int edge){
        int indexA = cornerIndexAFromEdge[edge];
        int indexB = cornerIndexBFromEdge[edge];

        // midpoint
        glm::vec3 midpoint = (cubeCorners[indexA] + cubeCorners[indexB]);

        midpoint *= 0.5;
        return midpoint;
    };

    /*Calculates normale for every posible triangle ~256*3 triangles*/
    std::vector<std::vector<glm::vec3>> triangulationNormals;
    for(std::vector<int> triangles : triangulation){
        std::vector<glm::vec3> normals;
        for(int i = 0; i < triangles.size(); i+=3){
            if(triangles[i] == -1) break;

            glm::vec3 p0 = getEdgeMidPoint(triangles[i]);
            glm::vec3 p1 = getEdgeMidPoint(triangles[i+1]);
            glm::vec3 p2 = getEdgeMidPoint(triangles[i+2]);

            glm::vec3 U = p1 - p0, V = p2 - p0;
            glm::vec3 normal(U.y*V.z - U.z*V.y, U.z*V.x - U.x*V.z, U.x*V.y - U.y*V.x);
            normals.push_back(-normal);
        }
        triangulationNormals.push_back(normals);
    }

    /* Getting all vertices */
    std::vector<float> vertices;
    /*Iterate for each voxel, checking the state of the 8 surrounding voxels to determine voxel topology*/
    for(int i = 0; i < controller.rows-1; i++){
        for(int j = 0; j < controller.cols-1; j++){
            for(int k = 0; k < controller.planes-1; k++){
                int voxelID = worldIdx(i, j, k, controller.rows, controller.rows, controller.rows);

                unsigned int v0 = voxel_grid[worldIdx(i, j, k, controller.rows, controller.rows, controller.rows)];
                unsigned int v1 = voxel_grid[worldIdx(i+1, j, k, controller.rows, controller.rows, controller.rows)] << 1;
                unsigned int v2 = voxel_grid[worldIdx(i+1, j, k+1, controller.rows, controller.rows, controller.rows)] << 2;
                unsigned int v3 = voxel_grid[worldIdx(i, j, k+1, controller.rows, controller.rows, controller.rows)] << 3;
                unsigned int v4 = voxel_grid[worldIdx(i, j+1, k, controller.rows, controller.rows, controller.rows)] << 4;
                unsigned int v5 = voxel_grid[worldIdx(i+1, j+1, k, controller.rows, controller.rows, controller.rows)] << 5;
                unsigned int v6 = voxel_grid[worldIdx(i+1, j+1, k+1, controller.rows, controller.rows, controller.rows)] << 6;
                unsigned int v7 = voxel_grid[worldIdx(i, j+1, k+1, controller.rows, controller.rows, controller.rows)] << 7;
                
                unsigned int mask = v0 | v1 | v2 | v3 | v4 | v5 | v6 | v7; /*configuration index*/

                std::vector<int> edges = triangulation[mask];  /*all edges from configuration, edges will become points*/
                
                
                for(int index=0; index<edges.size(); index+=3){
                    if(edges[index] == -1) break;

                    //get points
                    std::vector<glm::vec3> midpoints;
                    for(int mid=0; mid<3; mid++) midpoints.push_back(getEdgeMidPoint(edges[index+mid]));

                    // get normal
                    glm::vec3 normal = triangulationNormals[mask][index/3];

                    //insert points with normals
                    for(glm::vec3 midpoint : midpoints){
                        vertices.insert(vertices.end(), {midpoint.x+points_3d[voxelID*3], midpoint.y+points_3d[voxelID*3+1], midpoint.z+points_3d[voxelID*3+2],
                                                        normal.x, normal.y, normal.z});
                    }
                }

            }
        }
    }

    std::cout << "finished setting up vertices\n";

    /*Vertex Buffer Object and Vertex Array Object creation*/
    unsigned int VBOs[3], VAOs[3], EBO;
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);
    glGenBuffers(1, &EBO);

    /*binding vertices array*/
    controller.bind_load_normals_buffer(VBOs, VAOs, sizeof(float)*vertices.size(), vertices.data(), 0);


    /*Texture for saving the depth buffer*/
    const unsigned int SHADOW_WIDTH = WIDTH, SHADOW_HEIGHT = HEIGHT;
    
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);

    glDrawBuffer(GL_NONE);
    
    /*initial view matrices*/
    glm::mat4 projection_3d    = glm::mat4(1.0f);
    glm::mat4 view             = glm::mat4(1.0f);
    glm::mat4 projection       = glm::mat4(1.0f);
    projection_3d = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    /*Depth matrices*/
    glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 0.5f);
    float near_plane = -10.0f, far_plane = 30.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);  
    glm::mat4 lightView = glm::lookAt(lightPos, 
                            glm::vec3( 0.0f, 0.0f,  0.0f), 
                            glm::vec3( 0.0f, 1.0f,  0.0f));

    glm::mat4 lightSpaceMatrix = lightProjection * lightView; 
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT); 

    /*Rendering function*/
    auto render = [&](){
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*First we render to the buffer*/
        unsigned int lightSpaceMatrixLocation = glGetUniformLocation(depth_program, "lightSpaceMatrix");
        glUseProgram(depth_program);
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAOs[0]);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size()/6); 

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        /*Reset scene*/
        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /*Now the actual render*/
        glUseProgram(shader_program_3d);

        /*update some state variables*/
        controller.camera.update();

        /* get uniform locations */
        unsigned int color_loc = glGetUniformLocation(shader_program_3d, "uniColor");
        unsigned int lightPosLoc = glGetUniformLocation(shader_program_3d, "lightPos");
        unsigned int intensityLoc = glGetUniformLocation(shader_program_3d, "light_intensity");
        unsigned int viewLoc  = glGetUniformLocation(shader_program_3d, "view");
        unsigned int projLoc  = glGetUniformLocation(shader_program_3d, "projection");
        unsigned int shadowLoc  = glGetUniformLocation(shader_program_3d, "shadowMap");
        lightSpaceMatrixLocation = glGetUniformLocation(shader_program_3d, "lightSpaceMatrix");


        /* updates uniforms */
        glUniform3fv(color_loc, 1, controller.cell_color);
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform1f(intensityLoc, 1.0);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(controller.camera.get_camera_view()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_3d));
        glUniform1i(shadowLoc, 0);
        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/6); 


        
        /*Draws cell instances*/
        //glDrawArrays(GL_TRIANGLES, 0, vertices.size()/6); 
        
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

