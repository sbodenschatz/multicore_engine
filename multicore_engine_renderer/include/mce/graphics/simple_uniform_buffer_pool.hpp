/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_

#include <mce/graphics/simple_uniform_buffer.hpp>
#include <vector>

namespace mce {
namespace graphics {

class simple_uniform_buffer_pool {
	device* dev_;
	device_memory_manager_interface* mem_mgr_;
	destruction_queue_manager* dqm_;
	vk::DeviceSize buffer_size;
	std::vector<simple_uniform_buffer> buffers_;

public:
	simple_uniform_buffer_pool(device& dev, device_memory_manager_interface& mem_mgr,
							   destruction_queue_manager* destruction_manager, vk::DeviceSize buffer_size);
	~simple_uniform_buffer_pool() = default;

	simple_uniform_buffer_pool(const simple_uniform_buffer_pool&) = delete;
	simple_uniform_buffer_pool& operator=(const simple_uniform_buffer_pool&) = delete;
	simple_uniform_buffer_pool(simple_uniform_buffer_pool&&) noexcept = default;
	simple_uniform_buffer_pool& operator=(simple_uniform_buffer_pool&&) noexcept = default;

	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo store(const T& value);
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo try_store(const T& value);
	void reset();
	void flush();
	vk::DeviceSize available_space() const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_ */
