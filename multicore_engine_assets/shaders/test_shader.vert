#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out gl_PerVertex {
    vec4 gl_Position;
};

out vec3 color;

void main() {
	color = in_color;
    gl_Position = vec4(in_position,1.0);
}
