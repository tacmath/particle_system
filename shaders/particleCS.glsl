#version 460
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer posBuffer {
    float [][3] posision; //maybe use a vec3 to test perf but it is 4 float (16 bytes)
};

layout(std430, binding = 1) buffer velBuffer {
    float [][3] velocity;
};

void main() {
    uint id = gl_GlobalInvocationID.x;
    posision[id][0] -= 0.01f;
}