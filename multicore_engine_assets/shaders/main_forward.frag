#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 output_color;

layout(location = 0) in vec2 var_uv;
layout(location = 1) in mat3 var_tangent_space;

layout(set=1,binding = 0) uniform sampler2D albedo_tex;
layout(set=1,binding = 1) uniform sampler2D normal_tex;
layout(set=1,binding = 2) uniform sampler2D material_tex;
layout(set=1,binding = 3) uniform sampler2D emission_tex;

void main() {
	vec2 uv = vec2(var_uv.x,1.0-var_uv.y);
	vec2 tex_normal = texture(normal_tex,uv).ag * 2.0 - 1.0;
	vec3 material_params = texture(material_tex,uv).rgb;
	float metallic = material_params.r;
	float roughness = material_params.g;
	float ao = material_params.b;
	vec3 normal = vec3(tex_normal,sqrt(1-tex_normal.x*tex_normal.x-tex_normal.y*tex_normal.y));
	normal = normalize(var_tangent_space * normal);
	//output_color = texture(albedo_tex,uv);
	output_color = vec4(abs(normal)*ao,1.0);
	//output_color = vec4(ao,ao,ao,1.0);
}
