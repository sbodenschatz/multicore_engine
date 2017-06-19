/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device_memory_manager.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_
#define GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_

/**
 * \file
 * Defines a simple memory manager for device memory.
 */

#include <mce/graphics/device_memory_handle.hpp>
#include <mce/graphics/vk_mock_interface.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// Provides a simple first-fit freelist-based memory manager for device memory.
/**
 * Organizes device memory in a pool per memory requirements set.
 * Each pool consists of blocks with a fixed size, from which allocations of differing sizes are handed out.
 * Objects bigger than the block size get their own block of device memory allocated.
 */
class device_memory_manager final : public device_memory_manager_interface {
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
		vk_mock_interface::device_memory_wrapper memory_object;
		vk::DeviceSize size;
		vk::MemoryPropertyFlags flags;
		uint32_t memory_type;
		std::vector<freelist_entry> freelist;
		device_memory_block(int32_t id, vk_mock_interface::device_memory_wrapper&& memory_object,
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
	/// Constructs a memory manager for the given device using the given block size.
	device_memory_manager(device* dev, vk::DeviceSize block_size);

	/// \brief Releases all bound resources, user code must ensure, that device memory is not used after it's
	/// memory manager is destroyed.
	~device_memory_manager();

	/// Requests memory satisfying the given requirements from the manager.
	virtual device_memory_allocation
	allocate(const vk::MemoryRequirements& memory_requirements,
			 vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal) override;
	/// Returns the given memory allocation back to the manager.
	virtual void free(const device_memory_allocation& allocation) override;
	/// Releases device memory by releasing empty blocks, keeping at most the given amount of blocks per pool.
	void cleanup(unsigned int keep_per_memory_type = 0);
	/// Determines the complete capacity of the memory managed by this memory manager.
	vk::DeviceSize capacity() const;

	virtual device* associated_device() const override {
		return dev;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_DEVICE_MEMORY_MANAGER_HPP_ */
