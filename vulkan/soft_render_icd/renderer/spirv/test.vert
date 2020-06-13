#version 450 core

layout (location = 0) in vec3 inA;
layout (location = 0) out vec3 outA;

void main(){
    gl_Position = vec4(0);
    outA = inA;
}
