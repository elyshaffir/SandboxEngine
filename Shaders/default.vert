#version 450

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 inColor;

layout(push_constant) uniform PushConstantData {
    vec2 offset;
    vec3 color;
} pushConstantData;

void main()
{
    gl_Position = vec4(position + pushConstantData.offset, 0.0, 1.0);
}
