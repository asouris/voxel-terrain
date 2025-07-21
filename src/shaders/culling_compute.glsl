#version 430

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
layout(std430, binding = 0) buffer Offsets {
    float data[];
};
layout(std430, binding = 1) buffer Draw {
    int draw[];
};
layout(binding=2) uniform atomic_uint counter;

uniform int globalX, globalZ;
uniform int chunkSize;
uniform float voxelSize;

void emitVoxel(int x, int y, int z){
    uint idx        = atomicCounterIncrement(counter);

    idx             = idx * 3;
    data[idx]       = (globalX * chunkSize  + float(x)) * voxelSize;
    data[idx + 1]   = float(y) * voxelSize;
    data[idx + 2]   = (globalZ * chunkSize  + float(z)) * voxelSize;
}

void main() {
    int x           = int(gl_GlobalInvocationID.x);
    int y           = int(gl_GlobalInvocationID.y);
    int z           = int(gl_GlobalInvocationID.z);

    if (draw[x + y * chunkSize + z * chunkSize * chunkSize * 2] == 0) return;

    uvec3 nbr[6] = uvec3[6](
        gl_GlobalInvocationID + uvec3( 1,0,0), gl_GlobalInvocationID + uvec3(-1,0,0),
        gl_GlobalInvocationID + uvec3(0, 1,0), gl_GlobalInvocationID + uvec3(0,-1,0),
        gl_GlobalInvocationID + uvec3(0, 0,1), gl_GlobalInvocationID + uvec3(0, 0,-1)
    );

    if (x == 0 || x == chunkSize-1 || z == 0 || z == chunkSize-1 || y == 0){
        emitVoxel(x, y, z);
        return;
    }
    
    for(int i=0;i<6;i++){
        uint nx     = nbr[i].x;
        uint ny     = nbr[i].y;
        uint nz     = nbr[i].z;
        uint nidx   = nx + ny * chunkSize + nz * chunkSize * chunkSize;

        if (draw[nidx] == 0){
            emitVoxel(x, y, z);
            return;
        }
    }
    
}
