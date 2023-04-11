#version 400
layout (location = 0) in vec3 aPos;

uniform mat4 MVP;

void main()
{   
    gl_Position = vec4(aPos, 1.0f);
}