#version 460 core

layout (local_size_x = 10, local_size_y = 10, local_size_z = 10) in;

// binds image
layout(rgba32f, binding = 0) uniform image3D imgOutput;

int seed = 69;
int octaves = 6;
float persistence = 0.6;
float lacunarity = 0.8;

vec3 fade(vec3 t) {
    return t * t * (3.0 - 2.0 * t);
}

float grad(int hash, vec3 p) {
    int h = hash & 7;        // toma 3 bits
    vec3 grad = (h < 4) ? vec3(1.0,1.0, 1.0) : vec3(-1.0,-1.0, -1.0);
    if ((h & 1) != 0) grad.x = -grad.x;
    if ((h & 2) != 0) grad.y = -grad.y;
    return dot(grad, p);
}

int permute(int x) {
    return (((x + seed) * 34) + 1) * (x + seed) % 289;
}

float perlin3d(vec3 P) {
    ivec3 Pi0 = ivec3(floor(P));
    ivec3 Pi1 = Pi0 + ivec3(1);
    vec3 Pf0 = fract(P);                        //frac
    vec3 Pf1 = Pf0 - vec3(1);

    ivec4 ix = ivec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    ivec4 iy = ivec4(vec2(Pi0.y), vec2(Pi1.y));
	ivec4 iz0 = ivec4(Pi0.z);
	ivec4 iz1 = ivec4(Pi1.z);

    ivec4 ixy = ivec4(permute(permute(ix.x) + iy.x));
    vec4 ixy0 = vec4(permute(ixy.y + iz0.y));
    vec4 ixy1 = vec4(permute(ixy.y + iz1.z));

    vec4 gx0 = ixy0 * 1.0 / 7.0;
    vec4 gy0 = fract(floor(gx0) * 1.0 / 7.0) - vec4(0.5); //frac
    gx0 = fract(gx0);
    vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(vec4(0), gx0) - vec4(0.5));
    gy0 -= sz0 * (step(vec4(0), gy0) - vec4(0.5));

    vec4 gx1 = ixy1 * vec4(1.0 / 7.0);
    vec4 gy1 = fract(floor(gx1) * vec4(1.0 / 7.0)) - vec4(0.5);
    gx1 = fract(gx1);
    vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(vec4(0), gx1) - vec4(0.5));
    gy1 -= sz1 * (step(vec4(0), gy1) - vec4(0.5));

    // vec3 g000 = vec3(gx0.x, gy0.x, gz0.x);
    // vec3 g100 = vec3(gx0.y, gy0.y, gz0.y);
    // vec3 g010 = vec3(gx0.z, gy0.z, gz0.z);
    // vec3 g110 = vec3(gx0.w, gy0.w, gz0.w);
    // vec3 g001 = vec3(gx1.x, gy1.x, gz1.x);
    // vec3 g101 = vec3(gx1.y, gy1.y, gz1.y);
    // vec3 g011 = vec3(gx1.z, gy1.z, gz1.z);
    // vec3 g111 = vec3(gx1.w, gy1.w, gz1.w);

    vec3 fade_xyz = fade(Pf0); //FADE
    vec4 n_z = mix(gx0, gx1, fade_xyz.z);
    vec2 n_yz = mix(vec2(n_z.x, n_z.y), vec2(n_z.z, n_z.w), fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return n_xyz ;//* 2.2;



}

float fractalPerlin(vec3 P) {
    float amplitude = 1.0;
    float frequency = 1.0;
    float sum = 0.0;
    float norm = 0.0;
    for (int i = 0; i < octaves; i++) {
        sum += amplitude * perlin3d(P * frequency);
        norm += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }
    return sum / norm;
}

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

float scale = 1.05;
float heightOffset = 0.;

void main() {

    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);

    float h       = noise(pixel_coords*0.01);

    float h2 = noise(pixel_coords*0.05);

    imageStore(imgOutput, pixel_coords, vec4(h) + h2/5 );
    
}
