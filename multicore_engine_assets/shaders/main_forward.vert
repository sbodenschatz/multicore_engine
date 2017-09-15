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
	vec3 cam_pos;
	uint active_lights;
	per_point_light_uniforms forward_lights[max_forward_lights];
} scene;

layout(push_constant) uniform per_object_push_constants{
	mat4 model;
} obj_pc;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_tangent;
layout(location = 2) in vec3 in_bitangent;
layout(location = 3) in vec3 in_normal;
layout(location = 4) in vec2 in_uv;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 var_uv;
layout(location = 1) out mat3 var_tangent_space;
layout(location = 4) out vec4 var_world_pos;

void main() {
	var_uv = in_uv;
	var_tangent_space = mat3(obj_pc.model) * mat3(in_tangent,in_bitangent,in_normal);
	var_world_pos = obj_pc.model * vec4(in_position,1.0);
	gl_Position = scene.projection * scene.view * var_world_pos;
}
