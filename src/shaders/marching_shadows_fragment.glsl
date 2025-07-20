#version 330 core

out vec4 FragColor;                         /*final frag color*/

in vec3 fragPos;                            
in vec3 fragNormal;                         /*fragment normal*/
in vec4 FragPosLightSpace;

uniform vec3 uniColor;                      /*cell color*/
uniform float light_intensity;              /*directional light intensity*/
uniform sampler2D shadowMap;
uniform vec3 lightPos;



vec3 computeDirectionalLight(float shad) {
    vec3 light_ambient = vec3(0.5, 0.5, 0.5);
    vec3 light_diffuse = vec3(1, 1, 1);

    //ambient
    vec3 ambient = light_ambient;

    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);

    float diff = max(dot(lightDir, normalize(fragNormal)), 0.0f);
    vec3 diffuse = light_diffuse * diff;

    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    return (ambient + (1.0 - shad) * diffuse) * uniColor;

}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

    return shadow;
}

void main()
{   
    float shadow = ShadowCalculation(FragPosLightSpace);

    FragColor = vec4(computeDirectionalLight(shadow) * light_intensity, 1.0f);
    
}