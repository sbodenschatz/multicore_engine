#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 output_color;

layout(location = 0) in vec2 var_uv;
layout(location = 1) in mat3 var_tangent_space;
layout(location = 4) in vec4 var_world_pos;

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

layout(set=1,binding = 0) uniform sampler2D albedo_tex;
layout(set=1,binding = 1) uniform sampler2D normal_tex;
layout(set=1,binding = 2) uniform sampler2D material_tex;
layout(set=1,binding = 3) uniform sampler2D emission_tex;

vec3 fresnel_schlick(float cos_theta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

void main() {
	vec2 uv = vec2(var_uv.x,1.0-var_uv.y);
	vec2 tex_normal = texture(normal_tex,uv).ag * 2.0 - 1.0;
	vec3 material_params = texture(material_tex,uv).rgb;
	vec3 albedo = texture(albedo_tex,uv).rgb;
	float metallic = material_params.r;
	float roughness = material_params.g;
	float ao = material_params.b;
	vec3 normal = vec3(tex_normal,sqrt(1-tex_normal.x*tex_normal.x-tex_normal.y*tex_normal.y));
	normal = normalize(var_tangent_space * normal);
	vec3 view = normalize(scene.cam_pos-var_world_pos.xyz);
	vec3 light_sum = vec3(0.0);
	for(uint i = 0; i<scene.active_lights; ++i){
		vec3 light_dir = scene.forward_lights[i].position-var_world_pos.xyz;
		float attenuation = 1.0 / dot(light_dir,light_dir);
		light_dir = normalize(light_dir);
		vec3 half_way = normalize(view + light_dir);
		float cos_theta = max(dot(half_way, view), 0.0);
		vec3 radiance = scene.forward_lights[i].color * scene.forward_lights[i].brightness * attenuation;
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, albedo, metallic);
		vec3 F = fresnel_schlick(max(dot(half_way, view), 0.0), F0);
		light_sum += F;
		//light_sum+=vec3(cos_theta*radiance);
		//light_sum += radiance;
	}
	output_color = vec4(light_sum,1.0);
	//output_color = vec4(view,1.0);
}
