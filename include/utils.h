

#include <ostream>
#include <vector>

/*imgui*/
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

/*glad/opengl*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*gl math*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



int worldIdx(int i, int j, int k, const int N, const int M, const int D);
std::vector<float> getCubeVertices (float cell_size);


/** Represents a Camera, manges view matrix logic */
struct Camera {

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);   /* position of camera*/
    glm::vec3 focus =    glm::vec3(0.0f, 0.0f, 0.0f);   /* target of camera*/
    double distance = 3;                                /* distance from target*/
    double theta = 45;                                  /* vertical angle*/
    double phi = 0;                                     /* horizontal angle*/

    int keys[4] = {0, 0, 0, 0};                         /* hold active keys*/

    /** Updates angles of view according to pressed keys, 
     * also position of camera according to angles*/
    void update();                  

    /** Gets the view matrix from position and focus vectors*/
    glm::mat4 get_camera_view();

};

/* Holds all variables that control the simulations, also any function that updates those variables*/
struct Controller{
    /* simulation constants */
    int WIDTH, HEIGHT;
    int rows, cols, planes;
    float SIM_SCALE = 1;          /* Scale of the simulation in comparison to window size*/
    float CELL_SIZE = 10.0;         /* Size of cell of pixels */
    float cell_gl_size;             /* Size of cell for OpenGL*/

    /* simulation state variables */
    int current_fps = 30;           /* simulation fps, used as simulation velocity */
    
    /* openGL uniforms */
    float cell_color[4] = {1, 1, 1, 1}; /* Color of cells */

    /* Camera */
    Camera camera = Camera();                   /* Simple view matrix controller*/
    

    /** A controller for a window of width and height given */
    Controller(int WIDTH, int HEIGHT);


    /* SHADERS FUNCTIONS */

    /** Loads a shader from a path.
     * @param path path to shader
     * @return shader identifier
    */
    unsigned int load_shader(std::string path, bool shader_type);

    unsigned int get_compute_program(std::string path);

    /** Creates shader program from shaders, if said so, it deletes the used shaders afterwards.
     * @param vertex vertex shader already loaded
     * @param fragment fragment shader already loaded
     * @param flag if true deletes the shaders after using them.
     * @return shader program identifier
     */
    unsigned int create_shader_program(unsigned int vertex, unsigned int fragment);


    /* BUFFER FUNCTIONS */

    /** Gets the number of active cells on the result array and updates the buffer
     * @param positions buffer to be updated with the positions of active cells
     * @param result    array with the result from a Conway step 
     * @param coords    array with coordinates for every cube
     * @param N         amount of rows
     * @param M         amount of columns
     * @return          number of active cells after step
     */
    int update_with_step(unsigned int &positions, std::vector<int> &result, std::vector<float> coords, int N, int M, int D);
    
    /** Binds and loads a static buffer of floats
     * @param VBOS array of vertex buffer objects
     * @param VAOs array of vertex array objects
     * @param size size of data array in bytes
     * @param data data array 
     * @param index index of the buffer
    */
    void bind_load_static_buffer(unsigned int *VBOs, unsigned int *VAOs, int size, float *data, int index);
    
    /** Binds and loads a static buffer of floats with positions and normals
     * @param VBOS array of vertex buffer objects
     * @param VAOs array of vertex array objects
     * @param size size of data array in bytes
     * @param data data array 
     * @param index index of the buffer
    */
    void bind_load_normals_buffer(unsigned int *VBOs, unsigned int *VAOs, int size, float *data, int index);
    
    /** Binds and loads a dynamic buffer of floats to use as indices in instantiation
     * @param VBOS array of vertex buffer objects
     * @param VAOs array of vertex array objects
     * @param size size of data array in bytes
     * @param data data array 
     * @param index index of the buffer
    */
    void bind_load_indices_buffer(unsigned int *VBOs, unsigned int *VAOs, unsigned int EBO, int size, float *data, int indices_size, int *indices_data, int index);


    /* VERTICES FUNCTIONS */

    /** Calculates positions for each cube in openGL space [-1, 1]
     * @return float vector with positions, 3 values per position
    */
    std::vector<float> grid_points_3d();

    /* IMGUI LOOP */
    /** Sets and render Imgui window */ 
    void renderImgui(GLFWwindow* window, ImGuiIO &io);



    /* VOXELS */
    void addnRandomVoxels(std::vector<int> &voxel_grid, int n);
    std::vector<float> getActiveVoxels(std::vector<int> &voxel_grid, std::vector<float> &positions, int n);



};

/** Holds glfw window logic*/
class Window {
public:
    GLFWwindow *m_glfwWindow;   /* the glfw window*/
    ImGuiIO *io;                /* input/output manager for Imgui*/

    /** A Window manager for the simulation given by the controller
     * @param c A controller
     */
    Window(Controller &c);

    /** Manages key input */
    auto internal_key_callback(int key, int action)-> void;

    /** Manages mouse scroll input */
    auto internal_scroll_callback(double xoffset, double yoffset)->void;


private:
    Controller *controller;     /* a controller*/
    int WIDTH, HEIGHT;          /* window's width and height*/

    /** Sets up glfw window and configuration */
    void init_glfw_window(int width, int height, const char *title);

    /** Real key callback */
    inline static auto WindowKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)  -> void {
        
            Window *window = static_cast<Window*>(glfwGetWindowUserPointer(win));
            window->internal_key_callback(key, action);
    }

    /** Real mouse scroll callback */
    inline static auto WindowScrollCallback(GLFWwindow* win, double xoffset, double yoffset)  -> void {
        
            Window *window = static_cast<Window*>(glfwGetWindowUserPointer(win));
            window->internal_scroll_callback(xoffset, yoffset);
    }
    
};

