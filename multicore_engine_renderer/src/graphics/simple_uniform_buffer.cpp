/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/simple_uniform_buffer.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/simple_uniform_buffer.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace mce {
namespace graphics {

static_assert(detail::uniform_buffer_is_element_compatible<glm::quat>::value &&
					  detail::uniform_buffer_is_element_compatible<glm::vec2>::value &&
					  detail::uniform_buffer_is_element_compatible<glm::vec3>::value &&
					  detail::uniform_buffer_is_element_compatible<glm::vec4>::value,
			  "Condition for uniform types must not be too strict for common glm types to be usable.");

simple_uniform_buffer::simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
											 destruction_queue_manager* destruction_manager,
											 vk::DeviceSize size)
		: data_buffer_(dev, mem_mgr, destruction_manager, size, vk::BufferUsageFlagBits::eUniformBuffer,
					   vk::MemoryPropertyFlagBits::eHostVisible),
		  current_offset_{0}, dev_{dev.native_device()} {}

} /* namespace graphics */
} /* namespace mce */
