#version 460 core

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

float scale = 1.75;
float heightOffset = 0.;

void main() {
    //float h       = fractalPerlin();


    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	
    // value.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    // value.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	
    value.x = float(texelCoord.x) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    value.y = float(texelCoord.y) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    imageStore(imgOutput, texelCoord, vec4(gl_GlobalInvocationID.xy, -5, 0));
    
	
    //imageStore(imgOutput, texelCoord, vec4(0, h * scale + heightOffset, 0, 1.0));

}