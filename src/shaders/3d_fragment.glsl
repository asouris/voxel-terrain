#version 330 core

out vec4 FragColor;                         /*final frag color*/

in vec3 fragPos;                            
in vec3 fragNormal;                         /*fragment normal*/

uniform vec3 uniColor;                      /*cell color*/
uniform float light_intensity;              /*directional light intensity*/



vec3 computeDirectionalLight() {
    vec3 light_direction = vec3(-1, 1, 1);
    vec3 light_ambient = vec3(0.5, 0.5, 0.5);
    vec3 light_diffuse = vec3(1, 1, 1);

    //ambient
    vec3 ambient = light_ambient * uniColor;

    // diffuse
    float diff = max(dot(normalize(fragNormal), light_direction), 0.0f);
    vec3 diffuse = uniColor * light_diffuse * diff;

    return ambient+diffuse;
}

void main()
{   

    FragColor = vec4(computeDirectionalLight() * light_intensity, 1.0f);
    
}