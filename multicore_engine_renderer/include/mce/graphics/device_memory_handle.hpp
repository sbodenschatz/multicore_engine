/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device_memory_handle.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_MM_DEVICE_MEMORY_HANDLE_HPP_
#define GRAPHICS_MM_DEVICE_MEMORY_HANDLE_HPP_

/**
 * \file
 * Defines types for handling allocated units of device memory.
 */

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Defines the data structure to represent an allocated unit of memory.
struct device_memory_allocation {
	int32_t block_id; ///< id of the block required for returning the allocation to it's managing pool.
	vk::DeviceMemory memory_object; ///< The underlying memory object from which this allocation was made.
	vk::DeviceSize internal_offset; ///< Internal start offset of the allocation (including padding).
	vk::DeviceSize internal_size;   ///< Internal size of the allocation (including padding).
	/// Start offset of the allocated space conforming to the alignment requirement.
	vk::DeviceSize aligned_offset;
	vk::DeviceSize aligned_size; ///< Size of the aligned memory unit.

	/// Constructs an empty allocation, indicating a null-value.
	device_memory_allocation()
			: block_id(0), memory_object(), internal_offset(0), internal_size(0), aligned_offset(0),
			  aligned_size(0) {}
	/// \brief Constructs an allocation struct from the given block id, memory, offset and size, alignment is
	/// done later on by the memory manager.
	device_memory_allocation(int32_t block_id, vk::DeviceMemory memory_object, vk::DeviceSize offset,
							 vk::DeviceSize size)
			: block_id(block_id), memory_object(std::move(memory_object)), internal_offset(offset),
			  internal_size(size), aligned_offset(offset), aligned_size(size) {}
	/// Checks if the allocation is valid (not null).
	bool valid() const {
		return block_id != 0;
	};
};

template <typename Manager_Type>
class device_memory_handle {
private:
	Manager_Type* manager_ptr_;
	device_memory_allocation allocation_;

public:
	device_memory_handle(Manager_Type* manager_ptr, device_memory_allocation allocation)
			: manager_ptr_(manager_ptr), allocation_(std::move(allocation)) {}
	device_memory_handle(const device_memory_handle&) = delete;
	device_memory_handle& operator=(const device_memory_handle&) = delete;
	device_memory_handle(device_memory_handle&& other)
			: manager_ptr_(other.manager_ptr_), allocation_(other.allocation_) {
		other.manager_ptr_ = nullptr;
	}
	device_memory_handle& operator=(device_memory_handle&& other) {
		if(this == &other) return *this;
		if(manager_ptr_) {
			manager_ptr_->free(allocation_);
		}

		manager_ptr_ = other.manager_ptr_;
		allocation_ = other.allocation_;
		other.manager_ptr_ = nullptr;

		return *this;
	}
	~device_memory_handle() {
		if(manager_ptr_) {
			manager_ptr_->free(allocation_);
		}
	}
	vk::DeviceMemory memory() const {
		return allocation_.memory_object;
	}
	vk::DeviceSize offset() const {
		return allocation_.aligned_offset;
	}
};

template <typename Manager_Type>
device_memory_handle<Manager_Type> make_device_memory_handle(Manager_Type& manager,
															 const device_memory_allocation& allocation) {
	return device_memory_handle<Manager_Type>(&manager, allocation);
}

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_MM_DEVICE_MEMORY_HANDLE_HPP_ */
