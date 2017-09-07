#version 450
#extension GL_ARB_separate_shader_objects : enable

struct per_point_light_uniforms {
	vec3 position;
	float radius;
	vec3 color;
	float brightness;
};

const uint max_forward_lights = 64;

layout(set = 0,binding = 0) uniform per_scene {
	mat4 projection;
	mat4 view;
	uint active_lights;
	per_point_light_uniforms forward_lights[max_forward_lights];
} scene;

layout(push_constant) uniform per_object_push_constants{
	mat4 model;
} obj_pc;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 uv;

void main() {
	uv = in_uv;
	gl_Position = scene.projection * scene.view * obj_pc.model * vec4(in_position,1.0);
}