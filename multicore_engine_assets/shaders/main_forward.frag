#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 output_color;

layout(location = 0) in vec2 uv;

layout(set=1,binding = 0) uniform sampler2D albedo_tex;
layout(set=1,binding = 1) uniform sampler2D normal_tex;
layout(set=1,binding = 2) uniform sampler2D material_tex;
layout(set=1,binding = 3) uniform sampler2D emission_tex;

void main() {
    output_color = texture(albedo_tex,uv);
}