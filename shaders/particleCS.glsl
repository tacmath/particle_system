#version 460
layout (local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer posBuffer {
    float [][3] posision; //maybe use a vec3 to test perf but it is 4 float (16 bytes)
};

layout(std430, binding = 1) buffer velBuffer {
    float [][3] velocity;
};

const float DT = 0.03;

void main() {
    uint id = gl_GlobalInvocationID.x;
    vec3 pos = vec3( posision[id][0],  posision[id][1],  posision[id][2]);
    vec3 vel = vec3( velocity[id][0],  velocity[id][1],  velocity[id][2]);

    vel += DT * normalize(vec3(0) - pos);
	pos += DT * vel;

    
    posision[id][0] = pos.x;
    posision[id][1] = pos.y;
    posision[id][2] = pos.z;

    velocity[id][0] = vel.x;
    velocity[id][1] = vel.y;
    velocity[id][2] = vel.z;
}