#version 330 core

out vec4 FragColor;                         /*final frag color*/

in vec3 fragPos;                            
in vec3 fragNormal;                         /*fragment normal*/

uniform vec3 uniColor;                      /*cell color*/
uniform float light_intensity;              /*directional light intensity*/
uniform vec3 lightPos;


vec3 computeDirectionalLight() {
    vec3 light_ambient = vec3(0.3, 0.3, 0.3);
    vec3 light_diffuse = vec3(1, 1, 1);

    //ambient
    vec3 ambient = light_ambient ;

    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normalize(fragNormal)), 0.0f);
    vec3 diffuse =  light_diffuse * diff;

    return (ambient+diffuse) * uniColor;
}

void main()
{   

    FragColor = vec4(computeDirectionalLight() * light_intensity, 1.0f);
    
}