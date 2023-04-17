#version 400
layout (location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 VP;

uniform vec3 center = vec3(0);

uniform vec3 centerColor = vec3(1);

uniform vec3 baseColor = vec3(1, 0, 0);

uniform float centerSize = 1.0f;

void main()
{   
    gl_Position = VP * vec4(aPos, 1.0f);
    float ratio = smoothstep(0, centerSize, length(center - aPos)); //or smoothstep(0, 1, length(center - aPos))
    color = baseColor * ratio + (1 - ratio) * centerColor;
}