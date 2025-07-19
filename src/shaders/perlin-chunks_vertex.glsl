#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aOffset;
layout(location = 2) in vec3 aNormal;

out vec3 fragNormal;
flat out int colorId;

uniform mat4 view;
uniform mat4 projection;
uniform float voxelSize;

int seed = 17;
int octaves = 1;
float persistence = 1.75;
float lacunarity = 2.0;

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




vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}



vec2 lakeCenter  = vec2(0.0, 0.0);
float lakeRadius = 50;
float lakeDepth  = 1.5;

float scale = .7;
float heightOffset = -.25;

void main(){
    float h       = fractalPerlin(aOffset.xy) * scale + heightOffset;
    //float h       = snoise(aOffset.xy) * scale + heightOffset;

    float d = distance(aOffset.xy, lakeCenter);
    if (d < lakeRadius) h -= lakeDepth * (1.0 - (d / lakeRadius));

    vec4 worldPos = vec4(aPos + aOffset, 1.0);
    gl_Position   = projection * view * worldPos;

    fragNormal    = aNormal;
    colorId       = (h < aOffset.z) ? 0 : 1; 
}