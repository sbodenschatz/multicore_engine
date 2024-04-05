/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_

/**
 * \file
 * Defines the simple_uniform_buffer class.
 */

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

/// Provides a simple buffer that stores linearly allocated uniform data blocks using a host-mapped buffer.
/**
 * It can store values of different types with the limitation that stored types must have standard layout and
 * must be trivially copyable.
 */
class simple_uniform_buffer {
	buffer data_buffer_;
	vk::DeviceSize current_offset_;
	vk::Device dev_;

public:
	/// \brief Creates a simple_uniform_buffer of the given size on the given device allocating the memory
	/// from the given memory manager and using the given destruction_queue_manager to defer destruction until
	/// the memory is no longer used (after number of swapchain images many frames).
	simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
						  destruction_queue_manager* destruction_manager, vk::DeviceSize size);
	/// \brief Destroys the object and releases the underlying resources to the destruction_queue_manager
	/// given on construction.
	~simple_uniform_buffer() = default;

	/// Forbids copying.
	simple_uniform_buffer(const simple_uniform_buffer&) = delete;
	/// Forbids copying.
	simple_uniform_buffer& operator=(const simple_uniform_buffer&) = delete;
	/// Allows moving.
	simple_uniform_buffer(simple_uniform_buffer&&) noexcept = default;
	/// Allows moving.
	simple_uniform_buffer& operator=(simple_uniform_buffer&&) noexcept = default;

	/// Checks if the given data value can fit in the remaining space in the buffer.
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	bool can_fit(const T&) noexcept {
		void* addr = reinterpret_cast<char*>(data_buffer_.mapped_pointer()) + current_offset_;
		vk::DeviceSize space = data_buffer_.size() - current_offset_;
		return memory::align(alignof(T), sizeof(T), addr, space);
	}
	/// \brief Tries to store the given data value in the buffer and returns a vk::DescriptorBufferInfo for it
	/// if successful or a empty one (with null handle buffer) if it didn't fit.
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
	/// \brief Stores the given data value in the buffer and returns a vk::DescriptorBufferInfo for it or
	/// throws an exception if not successful.
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
	/// Invalidates all data in the pool and resets it to an empty state, so the full capacity can be reused.
	void reset() {
		current_offset_ = 0;
	}
	/// Flushes the mapped data from the host to the device memory if the memory is not host-coherent.
	void flush() {
		data_buffer_.flush_mapped(dev_);
	}
	/// Returns the remaining amount of available space for data in the buffer.
	vk::DeviceSize available_space() const {
		return data_buffer_.size() - current_offset_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_HPP_ */
