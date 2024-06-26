#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 output_color;

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 uv;

layout(binding = 1) uniform sampler2D tex;

void main() {
    output_color = texture(tex,uv);
}
