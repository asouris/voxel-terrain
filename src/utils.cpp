

#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.h"

/** Returns 1d coordinates from 3d coordinates
 * @param i x coordinate
 * @param j y coordinate
 * @param k z coordiante
 * @param N number of rows
 * @param M number of columns
 * @param D number of planes
 */
int worldIdx(int i, int j, int k, const int N, const int M, const int D){
    k = (k + D) % D;
    i = (i + N) % N;
    j = (j + M) % M;
	return k * N * M + i * M + j;
}

Controller::Controller(int width, int height){
    rows = width * SIM_SCALE / CELL_SIZE, cols = height * SIM_SCALE / CELL_SIZE, planes = rows;
    WIDTH = width, HEIGHT = height;

    cell_gl_size = 2.0f * SIM_SCALE / (float)rows;
}


unsigned int Controller::load_shader(std::string path, bool shader_type){
    /*reading shader*/
    std::ifstream shaderInput;
    shaderInput.open(path);
    std::stringstream strStream;
    strStream << shaderInput.rdbuf() << "\n\0"; //read the file
    const std::string& tmp = strStream.str();   
    const char *shader_source = tmp.c_str();

    shaderInput.close();

    /*loading shader into opengl*/
    int success;
    char infoLog[512];

    unsigned int shader;
    if(shader_type) shader = glCreateShader(GL_FRAGMENT_SHADER);
    else            shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int Controller::get_compute_program(std::string path){
    /*reading shader*/
    std::ifstream shaderInput;
    shaderInput.open(path);
    std::stringstream strStream;
    strStream << shaderInput.rdbuf() << "\n\0"; //read the file
    const std::string& tmp = strStream.str();   
    const char *shader_source = tmp.c_str();

    shaderInput.close();

    /*loading shader into opengl*/
    int success;
    char infoLog[512];

    unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
   

    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // shader Program
    unsigned int ID = glCreateProgram();
    glAttachShader(ID, shader);
    glLinkProgram(ID);
    // check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return ID;

}

unsigned int Controller::create_shader_program(unsigned int vertex, unsigned int fragment){
    int success;
    char infoLog[512];
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return program;
}

void Controller::bind_load_static_buffer(unsigned int *VBOs, unsigned int *VAOs, int size, float *data, int index){
    glBindVertexArray(VAOs[index]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void Controller::bind_load_normals_buffer(unsigned int *VBOs, unsigned int *VAOs, int size, float *data, int index){
    glBindVertexArray(VAOs[index]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void Controller::bind_load_indices_buffer(unsigned int *VBOs, unsigned int *VAOs, unsigned int EBO, int size, float *data, int indices_size, int *indices_data, int index){
    glBindVertexArray(VAOs[index]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

std::vector<float> Controller::grid_points_3d(){
    std::vector<float> vertices;
    for(int k = planes-1; k >= 0; k--){
        for(int i = 0 ; i < rows; i++){
            for(int j = cols-1; j >= 0; j--){
                vertices.insert(vertices.end(), {cell_gl_size*(float)i - 0.8f, cell_gl_size*(float)j - 0.8f, cell_gl_size*(float)k - 0.8f});
            }
        }
    }

    return vertices;
}

// void Controller::renderImgui(GLFWwindow* window, ImGuiIO &io){

//     ImGui_ImplOpenGL3_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
//     ImGui::NewFrame();
    
//     /*Contents of the window*/
//     {
//         ImGui::Begin("Controller");    
//         ImGui::SetWindowPos(ImVec2(WIDTH*0.6, 20));                     

//         ImGui::SliderInt("Velocity", &current_fps, 0, 60);  
        
//         ImGui::ColorEdit4("Color", cell_color);    

//         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
//         ImGui::End();
//     }


//     /*Actual rendering*/
//     ImGui::Render();
//     int display_w, display_h;
//     glfwGetFramebufferSize(window, &display_w, &display_h);
// }


/* VOXELS */

void Controller::addnRandomVoxels(std::vector<int> &voxel_grid, int n){
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distr(0, voxel_grid.size()-1);
    for(int i = 0; i < n; i++){
        int index = distr(gen);
        voxel_grid[index] = 1;
    }
}

std::vector<float> Controller::getActiveVoxels(std::vector<int> &voxel_grid, std::vector<float> &positions, int n){
    std::vector<float> r;
    for(int i = 0; i < voxel_grid.size(); i++){
        if(voxel_grid[i]){
            r.insert(r.end(), {positions[i*3], positions[i*3 + 1], positions[i*3 + 2]});
        }
    }

    return r;
}




void Camera::update(){
    if (keys[0]) phi-=1;
    if (keys[1]) phi+=1;
    if (keys[2]) theta-=1;
    if (keys[3]) theta+=1;

    if (theta > 90) theta = 90;
    else if(theta < 0) theta = 0.0001;

    position.x = distance * sin(glm::radians(theta)) * sin(glm::radians(phi));
    position.y = distance * cos(glm::radians(theta));
    position.z = distance * sin(glm::radians(theta)) * cos(glm::radians(phi));
}

glm::mat4 Camera::get_camera_view(){
    return glm::lookAt(position, focus, glm::vec3(0.0f, 1.0f, 0.0f));
}

Window::Window(Controller &c){

    WIDTH = c.WIDTH, HEIGHT = c.HEIGHT;

    init_glfw_window(WIDTH, HEIGHT, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    controller = &c;
    glfwSetWindowUserPointer(m_glfwWindow, this);

    glfwSetKeyCallback(m_glfwWindow, WindowKeyCallback);
    glfwSetScrollCallback(m_glfwWindow, WindowScrollCallback);

    /*ImGui setup*/
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // io = &ImGui::GetIO();
    // (void)io;
    // (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // ImGui::StyleColorsDark();

    // // Setup scaling
    // ImGuiStyle& style = ImGui::GetStyle();
    
    // // Setup Platform/Renderer backends
    // ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
    
    // ImGui_ImplOpenGL3_Init("#version 430");
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    

    /*OpenGL config*/
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
}

void Window::init_glfw_window(int width, int height, const char *title){
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    if (m_glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(0);
    }



    glfwMakeContextCurrent(m_glfwWindow);
    //gladLoadGL(glfwGetProcAddress);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }  
}

void Window::internal_key_callback(int key, int action){

    if(key == GLFW_KEY_UP && action == GLFW_PRESS && controller->current_fps < 60) controller->current_fps += 1;
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && controller->current_fps > 1) controller->current_fps -= 1;


    if (key == GLFW_KEY_A){
        if(action == GLFW_PRESS) controller->camera.keys[0] = 1;
        else if(action == GLFW_RELEASE) controller->camera.keys[0] = 0;
    }
    if (key == GLFW_KEY_D){
        if(action == GLFW_PRESS) controller->camera.keys[1] = 1;
        else if(action == GLFW_RELEASE) controller->camera.keys[1] = 0;
    }
    if (key == GLFW_KEY_S){
        if(action == GLFW_PRESS) controller->camera.keys[2] = 1;
        else if(action == GLFW_RELEASE) controller->camera.keys[2] = 0;
    }
    if (key == GLFW_KEY_W){
        if(action == GLFW_PRESS) controller->camera.keys[3] = 1;
        else if(action == GLFW_RELEASE) controller->camera.keys[3] = 0;
    }
}

void Window::internal_scroll_callback(double xoffset, double yoffset){

    controller->camera.distance += yoffset;
}