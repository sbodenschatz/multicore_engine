#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniform_buffer_object {
    mat4 model;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 color;

void main() {
	color = in_color;
    gl_Position = ubo.proj * ubo.model * vec4(in_position,1.0);
}
