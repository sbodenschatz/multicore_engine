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
	vec3 normal = vec3(tex_normal,sqrt(1-tex_normal.x*tex_normal.x-tex_normal.y*tex_normal.y));
	//vec3 normal = texture(normal_tex,uv).agb * vec3(2.0,2.0,1.0) - vec3(1.0,1.0,0.0);
	normal = normalize(var_tangent_space * normal);
	//normal = normalize(normal);
	//output_color = texture(albedo_tex,uv);
	output_color = vec4(abs(normal),1.0);
	//output_color = vec4(tex_normal,1.0);
	//output_color = vec4(var_tangent_space[2],1.0);
	//output_color = vec4(var_tangent_space[0],1.0);
	//output_color = vec4(var_tangent_space[1],1.0);
}
