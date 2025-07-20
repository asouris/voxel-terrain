#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 fragNormal;
	
uniform sampler2D screen;

void main()
{
    float noise = texture(screen, vec2(aOffset.x, aOffset.z)).x;  
    vec3 noiseVec = vec3(0, noise, 0);

    gl_Position = projection * view * vec4(aPos + aOffset, 1.0);
    fragNormal = aNormal;
    fragPos = vec3(aPos + aOffset);
}