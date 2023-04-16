#version 460
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer posBuffer {
    vec3 posision[];
};

layout(std430, binding = 1) buffer velBuffer {
    vec3 velocity[];
};

void main() {
    uint id = gl_GlobalInvocationID.x;
    posision[id].x += 0.01f;
}