#version 450
#extension GL_ARB_separate_shader_objects : enable

/*
 * Based on technique explained in
 *    https://learnopengl.com/#!PBR/Theory
 *    and
 *    https://learnopengl.com/#!PBR/Lighting
 */

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

const float PI = 3.14159265359;

vec3 fresnel_schlick(float cos_theta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}


float distribution_GGX(vec3 n, vec3 h, float roughness) {
	float a = roughness*roughness;
	float a_sq = a*a;
	float n_dot_h = max(dot(n, h), 0.0);
	float n_dot_h_sq = n_dot_h * n_dot_h;
	
	float nom = a_sq;
	float denom = (n_dot_h_sq * (a_sq - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return nom / denom;
}

float geometry_schlick_GGX(float n_dot_v, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = n_dot_v;
	float denom = n_dot_v * (1.0 - k) + k;
	return nom / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness) {
	float n_dot_v = max(dot(n, v), 0.0);
	float n_dot_l = max(dot(n, l), 0.0);
	float ggx2 = geometry_schlick_GGX(n_dot_v, roughness);
	float ggx1 = geometry_schlick_GGX(n_dot_l, roughness);
	
	return ggx1 * ggx2;
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
	vec3 light_sum = vec3(0.03) * albedo * ao;
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
		float NDF = distribution_GGX(normal, half_way, roughness);
		float G = geometry_smith(normal, view, light_dir, roughness);
		vec3 nominator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light_dir), 0.0) + 0.001; 
		vec3 specular = nominator / denominator;
		vec3 k_s = F;
		vec3 k_d = vec3(1.0) - k_s;
		k_d *= 1.0 - metallic;
		float n_dot_l = max(dot(normal, light_dir), 0.0);
		light_sum += (k_d * albedo / PI + specular) * radiance * n_dot_l;
	}
	light_sum = light_sum / (light_sum + vec3(1.0));
	//light_sum = pow(light_sum, vec3(1.0/2.2));
	output_color = vec4(light_sum,1.0);
}
