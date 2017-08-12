/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_

#include <mce/graphics/buffer.hpp>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

namespace detail {

template <typename T>
struct uniform_buffer_is_element_compatible {
	static constexpr bool value = std::is_trivially_copyable<T>::value && std::is_standard_layout<T>::value;
};

} // namespace detail

class simple_uniform_buffer {
	buffer data_buffer_;
	vk::DeviceSize current_offset_;

public:
	simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
						  destruction_queue_manager* destruction_manager, vk::DeviceSize size);
	~simple_uniform_buffer();

	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	bool can_fit(const T&) const;
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo store(const T& value);
	void reset();
	void flush();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_ */
