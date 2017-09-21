/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/uniforms_structs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_UNIFORMS_STRUCTS_HPP_
#define MCE_RENDERING_UNIFORMS_STRUCTS_HPP_

/**
 * Defines structs for uniform and push constant data.
 */

namespace mce {
namespace rendering {

/// Defines the uniform data required for each point light.
struct per_point_light_uniforms {
	glm::vec3 position; ///< The position of the light in world coordinates.
	float radius;		///< The radius of influence of the light.
	glm::vec3 color;	///< The color of the light.
	float brightness;   ///< The brightness of the light.
};

/// The maximum number of lights supported in the forward rendering pass.
constexpr size_t max_forward_lights = 64;

/// Defines the uniform data passed to the shader per scene.
struct per_scene_uniforms {
	glm::mat4 projection;										 ///< The projection matrix of the camera.
	glm::mat4 view;												 ///< The view matrix of the camera.
	glm::vec3 cam_pos;											 ///< The world position of the camera.
	uint32_t active_lights = 0;									 ///< The number of lights actually present.
	per_point_light_uniforms forward_lights[max_forward_lights]; ///< Lights supplied for forward rendering.
};

/// Defines the push constant data passed to the shader per object.
struct per_object_push_constants {
	glm::mat4 model; ///< The model matrix representing the transform applied to the object.
};

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_UNIFORMS_STRUCTS_HPP_ */
