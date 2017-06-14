/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/command_pool.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_COMMAND_POOL_HPP_
#define GRAPHICS_COMMAND_POOL_HPP_

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class command_pool {
private:
	device& owner_device_;
	vk::UniqueCommandPool native_command_pool_;

public:
	command_pool(device& dev, uint32_t queue_family_index, bool transient = false,
				 bool buffer_resettable = false);

	const vk::CommandPool& native_command_pool() {
		return *native_command_pool_;
	}
	vk::CommandPool native_command_pool() const {
		return native_command_pool_.get();
	}
	void reset(bool release_resources = false);
	vk::UniqueCommandBuffer allocate_primary_command_buffer();
	vk::UniqueCommandBuffer allocate_secondary_command_buffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_COMMAND_POOL_HPP_ */
