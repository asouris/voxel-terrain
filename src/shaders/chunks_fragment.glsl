#version 430 core

in vec3 fragNormal;

out vec4 outColor;

vec3 lightDirection = vec3(0.0, 0.0, -1.0);
vec3 lightColor     = vec3(0.8, 0.4, 0.4);
float lightStrength = 0.5;

float ambientStrength  = 0.2;
vec3 ambientColor      = vec3(0.3, 0.3, 0.3);

void main()
{
    vec3 ambient = ambientStrength * ambientColor;

    vec3 norm = normalize(fragNormal);
    vec3 direction = normalize(-lightDirection);
    vec3 diffuse = max(dot(norm, direction), 0.0) * (lightColor * lightStrength);

    vec3 result = ambient + diffuse;

    outColor = vec4(result, 1.0);
}
