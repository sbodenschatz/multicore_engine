/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/format_from_type.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_
#define MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_

/**
 * \file
 * Defines the format_from_type type function to obtain the Vulkan format for a given type.
 */

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Type function that allows lookup of the default Vulkan format (e.g. for attributes) of a given C++ type.
/**
 * This default template specifies the undefined format to signal that no format was specified for the type.
 */
template <typename T>
struct format_from_type {
	/// Format for a type unknown to this type function, equals vk::Format::eUndefined.
	static constexpr vk::Format default_format = vk::Format::eUndefined;
};

/// Specialization of format_from_type for scalar floating point values.
template <>
struct format_from_type<float> {
	/// Format for scalar floats = vk::Format::eR32Sfloat.
	static constexpr vk::Format default_format = vk::Format::eR32Sfloat;
};

/// Specialization of format_from_type for 2d-vector floating point values.
template <>
struct format_from_type<glm::vec2> {
	/// Format for 2d-float vectors = vk::Format::eR32G32Sfloat.
	static constexpr vk::Format default_format = vk::Format::eR32G32Sfloat;
};

/// Specialization of format_from_type for 3d-vector floating point values.
template <>
struct format_from_type<glm::vec3> {
	/// Format for 3d-float vectors = vk::Format::eR32G32B32Sfloat.
	static constexpr vk::Format default_format = vk::Format::eR32G32B32Sfloat;
};

/// Specialization of format_from_type for 4d-vector floating point values.
template <>
struct format_from_type<glm::vec4> {
	/// Format for 4d-float vectors = vk::Format::eR32G32B32A32Sfloat.
	static constexpr vk::Format default_format = vk::Format::eR32G32B32A32Sfloat;
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_ */
