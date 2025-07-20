#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aOffset;
layout(location = 2) in vec3 aNormal;

out vec3 fragNormal;

uniform mat4 view;
uniform mat4 projection;

void main(){
    vec4 worldPos = vec4(aPos + aOffset, 1.0);
    gl_Position   = projection * view * worldPos;

    fragNormal    = aNormal;
}