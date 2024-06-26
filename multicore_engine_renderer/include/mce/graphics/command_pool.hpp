/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/command_pool.hpp
 * Copyright 2016-2018 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_COMMAND_POOL_HPP_
#define GRAPHICS_COMMAND_POOL_HPP_

/**
 * \file
 * Defines the class encapsulating vulkan command pools.
 */

#include <array>
#include <mce/graphics/device.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// Provides the functionality of vulkan command pools.
/**
 * \warning This class is not inherently thread-safe and access to it must be externally synchronized.
 *
 * \warning The release of allocated command buffers must also be externally synchronized against other uses
 * of the command_pool.
 */
class command_pool {
private:
	device& owner_device_;
	vk::UniqueCommandPool native_command_pool_;

public:
	/// Creates a command pool for the given queue family on the given device with the given parameters.
	command_pool(device& dev, uint32_t queue_family_index, bool transient = false,
				 bool buffer_resettable = false);

	/// Allows access to the underlying command pool handle.
	const vk::CommandPool& native_command_pool() {
		return *native_command_pool_;
	}
	/// Allows access to the underlying command pool handle.
	vk::CommandPool native_command_pool() const {
		return native_command_pool_.get();
	}
	/// Resets all command buffers allocated from the pool, optionally releasing resources.
	void reset(bool release_resources = false);
	/// Allocates and returns a primary command buffer from the pool.
	vk::UniqueCommandBuffer allocate_primary_command_buffer();
	/// Allocates and returns a secondary command buffer from the pool.
	vk::UniqueCommandBuffer allocate_secondary_command_buffer();

	/// \brief Allocates and returns multiple primary command buffers from the pool with a compile-time-fixed
	/// number of buffers.
	template <uint32_t buffer_count>
	std::array<vk::UniqueCommandBuffer, buffer_count> allocate_primary_command_buffers() {
		std::array<vk::UniqueCommandBuffer, buffer_count> ubuf;
		vk::CommandBufferAllocateInfo ai(native_command_pool_.get(), vk::CommandBufferLevel::ePrimary,
										 buffer_count);
		std::array<vk::CommandBuffer, buffer_count> buf;
		vk::Result res = owner_device_->allocateCommandBuffers(&ai, buf.data());
		vk::PoolFree<vk::Device, vk::CommandPool, vk::DispatchLoaderStatic> del(owner_device_.native_device(),
																				native_command_pool_.get());
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateCommandBuffers");
		}
		for(uint32_t i = 0; i < buffer_count; ++i) {
			ubuf[i] = vk::UniqueCommandBuffer(buf[i], del);
		}
		return ubuf;
	}
	/// \brief Allocates and returns multiple secondary command buffers from the pool with a
	/// compile-time-fixed number of buffers.
	template <uint32_t buffer_count>
	std::array<vk::UniqueCommandBuffer, buffer_count> allocate_secondary_command_buffers() {
		std::array<vk::UniqueCommandBuffer, buffer_count> ubuf;
		vk::CommandBufferAllocateInfo ai(native_command_pool_.get(), vk::CommandBufferLevel::eSecondary,
										 buffer_count);
		std::array<vk::CommandBuffer, buffer_count> buf;
		vk::Result res = owner_device_->allocateCommandBuffers(&ai, buf.data());
		vk::PoolFree<vk::Device, vk::CommandPool, vk::DispatchLoaderStatic> del(owner_device_.native_device(),
																				native_command_pool_.get());
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateCommandBuffers");
		}
		for(uint32_t i = 0; i < buffer_count; ++i) {
			ubuf[i] = vk::UniqueCommandBuffer(buf[i], del);
		}
		return ubuf;
	}

	/// Allocates and returns multiple primary command buffers from the pool with a dynamic number of buffers.
	std::vector<vk::UniqueCommandBuffer> allocate_primary_command_buffers(uint32_t buffer_count);
	/// \brief Allocates and returns multiple secondary command buffers from the pool with a dynamic number of
	/// buffers.
	std::vector<vk::UniqueCommandBuffer> allocate_secondary_command_buffers(uint32_t buffer_count);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_COMMAND_POOL_HPP_ */
