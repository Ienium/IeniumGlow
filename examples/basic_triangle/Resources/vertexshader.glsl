#version 330 core

layout (location = 0) in vec2 vertexPosition_modelspace;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec4 color;

out vec4 vertex_color;

void main ()
{
    gl_Position.xy = vertexPosition_modelspace;
    gl_Position.w = 1.0;
    vertex_color = color;
}