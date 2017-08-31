/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/uniforms_structs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_UNIFORMS_STRUCTS_HPP_
#define MCE_RENDERING_UNIFORMS_STRUCTS_HPP_

namespace mce {
namespace rendering {

struct per_point_light_uniforms {
	glm::vec3 position;
	glm::vec3 color;
	float radius;
};

constexpr size_t max_forward_lights = 8;

struct per_scene_uniforms {
	glm::mat4 projection;
	glm::mat4 view;
	per_point_light_uniforms forward_lights[max_forward_lights];
};

struct per_object_push_constants {
	glm::mat4 model;
};

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_UNIFORMS_STRUCTS_HPP_ */
