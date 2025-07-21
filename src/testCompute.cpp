#include<iostream>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#include"glad/include/glad/glad.h"
#include"GLFW/glfw3.h"





const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;



GLfloat vertices[] =
{
	-1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
	-1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f , 0.0f, 1.0f, 0.0f,
};

GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};


const char* screenVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;
void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	UVs = uvs;
})";
const char* screenFragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D screen;
in vec2 UVs;
void main()
{
	FragColor = texture(screen, UVs);
})";
const char* screenComputeShaderSource = R"(#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

// binds image
layout(rgba32f, binding = 0) uniform image2D imgOutput;

int seed = 69;
int octaves = 6;
float persistence = 0.6;
float lacunarity = 0.8;

vec2 fade(vec2 t) {
    return t * t * (3.0 - 2.0 * t);
}

float grad(int hash, vec2 p) {
    int h = hash & 7;        // toma 3 bits
    vec2 grad = (h < 4) ? vec2(1.0,1.0) : vec2(-1.0,-1.0);
    if ((h & 1) != 0) grad.x = -grad.x;
    if ((h & 2) != 0) grad.y = -grad.y;
    return dot(grad, p);
}

int permute(int x) {
    return (((x + seed) * 34) + 1) * (x + seed) % 289;
}

float perlin2d(vec2 P) {
    ivec2 Pi = ivec2(floor(P)) & 255;
    vec2   Pf = fract(P);
    vec2   f  = fade(Pf);

    int A  = permute(Pi.x) + Pi.y;
    int AA = permute(A);
    int AB = permute(A + 1);
    int B  = permute(Pi.x + 1) + Pi.y;
    int BA = permute(B);
    int BB = permute(B + 1);

    float v00 = grad(permute(AA), Pf - vec2(0.0,0.0));
    float v10 = grad(permute(BA), Pf - vec2(1.0,0.0));
    float v01 = grad(permute(AB), Pf - vec2(0.0,1.0));
    float v11 = grad(permute(BB), Pf - vec2(1.0,1.0));

    float x1 = mix(v00, v10, f.x);
    float x2 = mix(v01, v11, f.x);
    return mix(x1, x2, f.y);
}

float fractalPerlin(vec2 P) {
    float amplitude = 1.0;
    float frequency = 1.0;
    float sum = 0.0;
    float norm = 0.0;
    for (int i = 0; i < octaves; i++) {
        sum += amplitude * perlin2d(P * frequency);
        norm += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return sum / norm;
}

float scale = 4;
float heightOffset = 0.;

void main() {


    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dims = imageSize(imgOutput);
	float x = -(float(pixel_coords.x * 2 - dims.x) / dims.x); // transforms to [-1.0, 1.0]
	float y = -(float(pixel_coords.y * 2 - dims.y) / dims.y); // transforms to [-1.0, 1.0]

    float h       = fractalPerlin(pixel_coords/2);

	
    // value.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    // value.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	
    //value.x = float(pixel_coords.x) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    //value.y = float(pixel_coords.y) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);

    imageStore(imgOutput, pixel_coords, vec4(h * scale + heightOffset));
    
	
    //imageStore(imgOutput, texelCoord, vec4(0, h * scale + heightOffset, 0, 1.0));

})";


int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    std::cout << "still no segfault2\n";

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Compute Shaders", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
	}
	// glfwMakeContextCurrent(window);
	// glfwSwapInterval(vSync);

    glfwMakeContextCurrent(window);
    //gladLoadGL(glfwGetProcAddress);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }

    std::cout << "still no segfault1\n";

	// if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	// {
	// 	std::cout << "Failed to initialize OpenGL context" << std::endl;
	// }
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    std::cout << "still no segfault3\n";


	GLuint VAO, VBO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	int simulationWIDTH = 400;
	int simulationHEIGHT = 400;

	GLuint screenTex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, simulationWIDTH, simulationHEIGHT);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);

	GLuint screenShaderProgram = glCreateProgram();
	glAttachShader(screenShaderProgram, screenVertexShader);
	glAttachShader(screenShaderProgram, screenFragmentShader);
	glLinkProgram(screenShaderProgram);

	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);

	std::ifstream shaderInput;
    shaderInput.open("shaders/perlin.glsl");
    std::stringstream strStream;
    strStream << shaderInput.rdbuf() << "\n\0"; //read the file
    const std::string& tmp = strStream.str();   
    const char *shader_source = tmp.c_str();

    shaderInput.close();


	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &shader_source, NULL);
	glCompileShader(computeShader);

	GLuint computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);


	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" << 
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";


	while (!glfwWindowShouldClose(window))
	{
		glUseProgram(computeProgram);
		glDispatchCompute(simulationWIDTH, simulationHEIGHT, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		glUseProgram(screenShaderProgram);
		glBindTextureUnit(0, screenTex);
		glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();
}