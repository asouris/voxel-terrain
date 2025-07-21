#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec4 FragPosLightSpace;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    fragNormal = aNormal;
    fragPos = vec3(aPos);
    FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}
