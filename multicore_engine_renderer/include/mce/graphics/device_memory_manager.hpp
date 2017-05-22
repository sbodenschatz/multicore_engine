/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device_memory_manager.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_
#define GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_

#include <mce/graphics/device_memory_handle.hpp>
#include <mce/graphics/unique_handle.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class device_memory_manager {
private:
	struct freelist_entry {
		vk::DeviceSize offset;
		vk::DeviceSize size;
		freelist_entry(vk::DeviceSize offset, vk::DeviceSize size) : offset(offset), size(size) {}
		device_memory_allocation try_allocate(const vk::MemoryRequirements& memory_requirements,
											  int32_t block_id, const vk::DeviceMemory& memory_object);
		bool mergeable(const freelist_entry& successor) const;
		void merge(freelist_entry& successor);
	};
	struct device_memory_block {
		int32_t id;
		unique_handle<vk::DeviceMemory, true> memory_object;
		vk::DeviceSize size;
		vk::MemoryPropertyFlags flags;
		uint32_t memory_type;
		std::vector<freelist_entry> freelist;
		device_memory_block(int32_t id, unique_handle<vk::DeviceMemory, true>&& memory_object,
							vk::DeviceSize size, vk::MemoryPropertyFlags flags, uint32_t memory_type);
		device_memory_allocation try_allocate(const vk::MemoryRequirements& memory_requirements,
											  vk::MemoryPropertyFlags required_flags);
		void free(const device_memory_allocation& allocation);
		bool empty() const;
	};

	device* dev;
	vk::DeviceSize block_size_;
	vk::PhysicalDeviceMemoryProperties physical_device_properties_;
	std::vector<device_memory_block> blocks_;
	std::vector<device_memory_block> separate_blocks_;
	int32_t next_block_id = 1;			 // 0 is invalid
	int32_t next_separate_block_id = -1; // 0 is invalid

public:
	device_memory_manager(device* dev, vk::DeviceSize block_size);
	~device_memory_manager();

	device_memory_allocation
	allocate(const vk::MemoryRequirements& memory_requirements,
			 vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal);
	void free(const device_memory_allocation& allocation);
	void cleanup(unsigned int keep_per_memory_type = 0);
	vk::DeviceSize capacity() const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_ */
