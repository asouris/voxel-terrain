#version 330 core

in vec3 fragNormal;

out vec4 out_color;

vec3 lightDirection = vec3(0.0, 0.0, -1.0);
vec3 lightColor     = vec3(0.8, 0.4, 0.4);
float lightStrength = 0.5;

float ambient_strength  = 0.2;
vec3 ambient_color      = vec3(0.3, 0.3, 0.3);

void main()
{
    vec3 ambient = ambient_strength * ambient_color;

    vec3 norm = normalize(fragNormal);
    vec3 direction = normalize(-lightDirection);
    vec3 diffuse = max(dot(norm, direction), 0.0) * (lightColor * lightStrength);

    vec3 result = ambient + diffuse;

    out_color = vec4(result, 1.0);
}
