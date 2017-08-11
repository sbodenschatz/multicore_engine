/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_

namespace mce {
namespace graphics {

class simple_uniform_buffer {
	buffer data_buffer_;
	vk::DeviceSize current_offset_;

public:
	simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
						  destruction_queue_manager* destruction_manager, vk::DeviceSize size);
	~simple_uniform_buffer();

	template <typename T, typename = std::enable_if<std::is_pod<T>::value>>
	bool can_fit(const T&) const;
	template <typename T, typename = std::enable_if<std::is_pod<T>::value>>
	vk::DescriptorBufferInfo store(const T& value);
	void reset();
	void flush();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_ */
