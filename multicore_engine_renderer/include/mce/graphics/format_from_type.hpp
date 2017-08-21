/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/format_from_type.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_
#define MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

template <typename T>
struct format_from_type {
	static constexpr vk::Format default_format = vk::Format::eUndefined;
};

template <>
struct format_from_type<float> {
	static constexpr vk::Format default_format = vk::Format::eR32Sfloat;
};

template <>
struct format_from_type<glm::vec2> {
	static constexpr vk::Format default_format = vk::Format::eR32G32Sfloat;
};

template <>
struct format_from_type<glm::vec3> {
	static constexpr vk::Format default_format = vk::Format::eR32G32B32Sfloat;
};

template <>
struct format_from_type<glm::vec4> {
	static constexpr vk::Format default_format = vk::Format::eR32G32B32A32Sfloat;
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FORMAT_FROM_TYPE_HPP_ */
