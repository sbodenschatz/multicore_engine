/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_

#include <mce/exceptions.hpp>
#include <mce/graphics/buffer.hpp>
#include <mce/memory/align.hpp>
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
	vk::Device dev_;

public:
	simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
						  destruction_queue_manager* destruction_manager, vk::DeviceSize size);
	~simple_uniform_buffer() = default;

	simple_uniform_buffer(const simple_uniform_buffer&) = delete;
	simple_uniform_buffer& operator=(const simple_uniform_buffer&) = delete;
	simple_uniform_buffer(simple_uniform_buffer&&) noexcept = default;
	simple_uniform_buffer& operator=(simple_uniform_buffer&&) noexcept = default;

	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	bool can_fit(const T&) noexcept {
		void* addr = reinterpret_cast<char*>(data_buffer_.mapped_pointer()) + current_offset_;
		vk::DeviceSize space = data_buffer_.size() - current_offset_;
		return memory::align(alignof(T), sizeof(T), addr, space);
	}
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo try_store(const T& value) noexcept {
		void* addr = reinterpret_cast<char*>(data_buffer_.mapped_pointer()) + current_offset_;
		vk::DeviceSize space = data_buffer_.size() - current_offset_;
		if(memory::align(alignof(T), sizeof(T), addr, space)) {
			std::memcpy(addr, &value, sizeof(T));
			auto offset =
					reinterpret_cast<char*>(addr) - reinterpret_cast<char*>(data_buffer_.mapped_pointer());
			addr = reinterpret_cast<char*>(addr) + sizeof(T);
			current_offset_ =
					reinterpret_cast<char*>(addr) - reinterpret_cast<char*>(data_buffer_.mapped_pointer());
			return {data_buffer_.native_buffer(), vk::DeviceSize(offset), sizeof(T)};
		} else {
			return {};
		}
	}
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo store(const T& value) {
		void* addr = reinterpret_cast<char*>(data_buffer_.mapped_pointer()) + current_offset_;
		vk::DeviceSize space = data_buffer_.size() - current_offset_;
		if(memory::align(alignof(T), sizeof(T), addr, space)) {
			std::memcpy(addr, &value, sizeof(T));
			auto offset =
					reinterpret_cast<char*>(addr) - reinterpret_cast<char*>(data_buffer_.mapped_pointer());
			addr = reinterpret_cast<char*>(addr) + sizeof(T);
			current_offset_ =
					reinterpret_cast<char*>(addr) - reinterpret_cast<char*>(data_buffer_.mapped_pointer());
			return {data_buffer_.native_buffer(), vk::DeviceSize(offset), sizeof(T)};
		} else {
			throw mce::resource_depleted_exception("Space in uniform buffer depleted.");
		}
	}
	void reset() {
		current_offset_ = 0;
	}
	void flush() {
		data_buffer_.flush_mapped(dev_);
	}
	vk::DeviceSize available_space() const {
		return data_buffer_.size() - current_offset_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_ */
