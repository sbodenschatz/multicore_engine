#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniform_buffer_object {
    mat4 model;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 uv;

void main() {
	color = abs(in_normal);
	uv = in_uv;
	gl_Position = ubo.proj * ubo.model * vec4(in_position,1.0);
}
