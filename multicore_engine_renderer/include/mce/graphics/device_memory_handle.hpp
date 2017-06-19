/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device_memory_handle.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
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
class device;

/// Defines the data structure to represent an allocated unit of memory.
struct device_memory_allocation {
	int32_t block_id; ///< id of the block required for returning the allocation to it's managing pool.
	vk::DeviceMemory memory_object; ///< The underlying memory object from which this allocation was made.
	vk::DeviceSize internal_offset; ///< Internal start offset of the allocation (including padding).
	vk::DeviceSize internal_size;   ///< Internal size of the allocation (including padding).
	/// Start offset of the allocated space conforming to the alignment requirement.
	vk::DeviceSize aligned_offset;
	vk::DeviceSize aligned_size; ///< Size of the aligned memory unit.
	void* mapped_pointer = nullptr;
	size_t mapping_count = 0;

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

/// Provides an interface for device memory managers to allocate and free allocations polymorphically.
class device_memory_manager_interface {
public:
	/// Enables polymorphic destruction.
	virtual ~device_memory_manager_interface() noexcept = default;
	/// Interface function to free allocations from any device memory manager.
	virtual void free(const device_memory_allocation& allocation) = 0;
	/// Interface function to request memory satisfying the given requirements from the manager.
	virtual device_memory_allocation
	allocate(const vk::MemoryRequirements& memory_requirements,
			 vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal) = 0;
	virtual device* associated_device() const = 0;
};

/// Provides a RAII wrapper for managing the lifetime of a device_memory_allocation and the associated memory.
/**
 * This class encapsulates an allocation and a pointer to the memory manager from which it came and returns
 * the memory allocation to the manager when the handle goes out of scope.
 */
class device_memory_handle {
private:
	device_memory_manager_interface* manager_ptr_;
	device_memory_allocation allocation_;

public:
	/// Constructs an empty handle.
	device_memory_handle() : device_memory_handle(nullptr, device_memory_allocation()) {}
	/// Constructs a handle for the given allocation.
	device_memory_handle(device_memory_manager_interface* manager_ptr, device_memory_allocation allocation)
			: manager_ptr_(manager_ptr), allocation_(std::move(allocation)) {}
	/// Forbids copying because shared ownership is not supported.
	device_memory_handle(const device_memory_handle&) = delete;
	/// Forbids copying because shared ownership is not supported.
	device_memory_handle& operator=(const device_memory_handle&) = delete;
	/// Allows moving handles.
	device_memory_handle(device_memory_handle&& other)
			: manager_ptr_(other.manager_ptr_), allocation_(other.allocation_) {
		other.manager_ptr_ = nullptr;
	}
	/// Allows moving handles.
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
	/// Destroys the handle and frees the allocated memory back to the manager.
	~device_memory_handle() {
		if(manager_ptr_) {
			manager_ptr_->free(allocation_);
		}
	}
	/// Allows access to the memory object.
	vk::DeviceMemory memory() const {
		return allocation_.memory_object;
	}
	/// Allows access to the offset of the allocation within the memory object.
	vk::DeviceSize offset() const {
		return allocation_.aligned_offset;
	}
	/// Allows access to the encapsulated allocation.
	const device_memory_allocation& allocation() const {
		return allocation_;
	}
	/// Frees the allocated memory back to the manager and leaves the handle empty.
	void reset() {
		if(manager_ptr_) {
			manager_ptr_->free(allocation_);
			allocation_ = device_memory_allocation();
			manager_ptr_ = nullptr;
		}
	}
};

/// \brief Creates a device_memory_handle from the given allocation and manager
inline device_memory_handle make_device_memory_handle(device_memory_manager_interface& manager,
													  const device_memory_allocation& allocation) {
	return device_memory_handle(&manager, allocation);
}

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_MM_DEVICE_MEMORY_HANDLE_HPP_ */
