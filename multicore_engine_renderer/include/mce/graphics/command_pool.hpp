/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/command_pool.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_COMMAND_POOL_HPP_
#define GRAPHICS_COMMAND_POOL_HPP_

/**
 * \file
 * Defines the class encapsulating vulkan command pools.
 */

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// Provides the functionality of vulkan command pools.
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
	/// Allocates returns a primary command buffer from the pool.
	vk::UniqueCommandBuffer allocate_primary_command_buffer();
	/// Allocates returns a secondary command buffer from the pool.
	vk::UniqueCommandBuffer allocate_secondary_command_buffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_COMMAND_POOL_HPP_ */
