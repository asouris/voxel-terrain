#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 fragNormal;
//out vec2 TexCoords;

//in vec2 TexCoords;
	
uniform sampler2D tex;

void main()
{
    vec3 texCol = texture(tex, aTexCoords).rgb;  

    gl_Position = projection * view * vec4(aPos + aOffset + texCol, 1.0);
    fragNormal = aNormal;
    fragPos = vec3(aPos + aOffset);
}