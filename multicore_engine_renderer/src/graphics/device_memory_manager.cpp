/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/graphics/device_memory_manager.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <cassert>
#include <mce/exceptions.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/vk_mock_interface.hpp>
#include <mce/graphics/window.hpp>
#include <mce/memory/align.hpp>
#include <mce/util/algorithm.hpp>
#include <numeric>
#include <stdexcept>

namespace mce {
namespace graphics {

device_memory_manager::device_memory_block::device_memory_block(
		int32_t id, vk_mock_interface::device_memory_wrapper&& memory_object, vk::DeviceSize size,
		vk::MemoryPropertyFlags flags, uint32_t memory_type)
		: id(id), memory_object(std::move(memory_object)), size(size), flags(flags), memory_type(memory_type),
		  freelist({{0, size}}) {}

device_memory_allocation
device_memory_manager::freelist_entry::try_allocate(const vk::MemoryRequirements& memory_requirements,
													int32_t block_id, const vk::DeviceMemory& memory_object) {
	if(memory_requirements.size > size) return device_memory_allocation();
	device_memory_allocation allocation(block_id, memory_object, offset, memory_requirements.size);
	if(memory::align_offset(memory_requirements.alignment, memory_requirements.size,
							allocation.aligned_offset, size)) {
		size -= memory_requirements.size;
		auto diff = allocation.aligned_offset - allocation.internal_offset;
		allocation.internal_size += diff;
		offset = allocation.internal_offset + allocation.internal_size;
		return allocation;
	} else {
		return device_memory_allocation();
	}
}
bool device_memory_manager::freelist_entry::mergeable(const freelist_entry& successor) const {
	return (offset + size) == successor.offset;
}
void device_memory_manager::freelist_entry::merge(freelist_entry& successor) {
	assert(mergeable(successor));
	size += successor.size;
}

device_memory_allocation
device_memory_manager::device_memory_block::try_allocate(const vk::MemoryRequirements& memory_requirements,
														 vk::MemoryPropertyFlags required_flags) {
	if((flags & required_flags) != required_flags) return device_memory_allocation();
	if(!(memory_requirements.memoryTypeBits & (1 << memory_type))) return device_memory_allocation();
	for(auto it = freelist.begin(); it != freelist.end(); ++it) {
		auto alloc = it->try_allocate(memory_requirements, id, memory_object.get());
		if(alloc.valid()) {
			if(it->size == 0) {
				// Remove 0-length freelist entry
				freelist.erase(it);
			}
			return alloc;
		}
	}
	return device_memory_allocation();
}

void device_memory_manager::free(const device_memory_allocation& allocation) {
	if(allocation.block_id == 0)
		return;						   // Invalid allocation
	else if(allocation.block_id > 0) { // Normal block
		auto it = std::find_if(blocks_.begin(), blocks_.end(),
							   [&](device_memory_block& block) { return block.id == allocation.block_id; });
		it->free(allocation);
	} else { // Separate block
		separate_blocks_.erase(
				std::remove_if(separate_blocks_.begin(), separate_blocks_.end(),
							   [&](device_memory_block& block) { return block.id == allocation.block_id; }),
				separate_blocks_.end());
	}
}

void device_memory_manager::device_memory_block::free(const device_memory_allocation& allocation) {
	// TODO Optimize by making use of the assumption that freelist is already sorted.
	freelist.emplace_back(allocation.internal_offset, allocation.internal_size);
	std::sort(freelist.begin(), freelist.end(),
			  [](freelist_entry& a, freelist_entry& b) { return a.offset < b.offset; });
	auto new_end =
			util::merge_adjacent_if(freelist.begin(), freelist.end(), std::mem_fn(&freelist_entry::mergeable),
									std::mem_fn(&freelist_entry::merge));
	freelist.erase(new_end, freelist.end());
}

bool device_memory_manager::device_memory_block::empty() const {
	if(freelist.size() != 1) return false;
	return freelist.front().size == size;
}

void device_memory_manager::cleanup(unsigned int keep_per_memory_type) {
	auto divider = std::stable_partition(blocks_.begin(), blocks_.end(),
										 [](const device_memory_block& blk) { return !blk.empty(); });
	size_t div_pos = std::distance(blocks_.begin(), divider);
	auto new_end = util::n_unique(divider, blocks_.end(),
								  [](const device_memory_block& a, const device_memory_block& b) {
									  return a.memory_type == b.memory_type;
								  },
								  keep_per_memory_type);
	blocks_.erase(new_end, blocks_.end());
	divider = blocks_.begin() + div_pos;
	std::inplace_merge(blocks_.begin(), divider, blocks_.end(),
					   [](const device_memory_block& a, const device_memory_block& b) {
						   return a.memory_type < b.memory_type;
					   });
}

device_memory_allocation device_memory_manager::allocate(const vk::MemoryRequirements& memory_requirements,
														 vk::MemoryPropertyFlags required_flags) {
	if(memory_requirements.size < block_size_) {
		for(auto& block : blocks_) {
			auto alloc = block.try_allocate(memory_requirements, required_flags);
			if(alloc.valid()) return alloc;
		}
		// No existing block can provide memory with the requested properties, allocate a new one:
		bool type_found = false;
		for(auto type_bits = memory_requirements.memoryTypeBits, mem_type = 0u;
			type_bits && mem_type < physical_device_properties_.memoryTypeCount;
			++mem_type, type_bits >>= 1) {
			if((type_bits & 1u) && ((physical_device_properties_.memoryTypes[mem_type].propertyFlags &
									 required_flags) == required_flags)) {
				type_found = true;
				vk::MemoryAllocateInfo ai;
				ai.allocationSize = block_size_;
				ai.memoryTypeIndex = mem_type;
				vk_mock_interface::device_memory_wrapper mem = vk_mock_interface::allocate_memory(dev, ai);
				if(!mem.get()) continue;
				auto insert_pos = std::lower_bound(
						blocks_.begin(), blocks_.end(), mem_type,
						[](const device_memory_block& blk, uint32_t type) { return blk.memory_type < type; });
				auto block_it = blocks_.emplace(insert_pos, next_block_id++, std::move(mem), block_size_,
												required_flags, mem_type);
				auto alloc = block_it->try_allocate(memory_requirements, required_flags);
				if(alloc.valid()) return alloc;
			}
		}
		if(type_found) {
			throw device_memory_allocation_exception(
					"Not enough memory with the specified properties of the compatible types.");
		} else {
			throw device_memory_allocation_exception(
					"No compatible memory type has the requested properties.");
		}
	} else {
		for(auto& block : separate_blocks_) {
			auto alloc = block.try_allocate(memory_requirements, required_flags);
			if(alloc.valid()) return alloc;
		}
		bool type_found = false;
		for(auto type_bits = memory_requirements.memoryTypeBits, mem_type = 0u;
			type_bits && mem_type < physical_device_properties_.memoryTypeCount;
			++mem_type, type_bits >>= 1) {
			if((type_bits & 1u) && ((physical_device_properties_.memoryTypes[mem_type].propertyFlags &
									 required_flags) == required_flags)) {
				type_found = true;
				vk::MemoryAllocateInfo ai;
				ai.allocationSize = memory_requirements.size;
				ai.memoryTypeIndex = mem_type;
				vk_mock_interface::device_memory_wrapper mem = vk_mock_interface::allocate_memory(dev, ai);
				if(!mem.get()) continue;
				auto insert_pos = std::lower_bound(
						separate_blocks_.begin(), separate_blocks_.end(), mem_type,
						[](const device_memory_block& blk, uint32_t type) { return blk.memory_type < type; });
				auto block_it = separate_blocks_.emplace(insert_pos, next_separate_block_id--, std::move(mem),
														 memory_requirements.size, required_flags, mem_type);
				auto alloc = block_it->try_allocate(memory_requirements, required_flags);
				if(alloc.valid())
					return alloc;
				else
					throw std::runtime_error("Allocation from single-allocation block failed.");
			}
		}
		if(type_found) {
			throw device_memory_allocation_exception(
					"Not enough memory with the specified properties of the compatible types.");
		} else {
			throw device_memory_allocation_exception(
					"No compatible memory type has the requested properties.");
		}
	}
}

device_memory_manager::device_memory_manager(device* dev, vk::DeviceSize block_size)
		: dev(dev), block_size_(block_size),
		  physical_device_properties_(vk_mock_interface::get_physical_dev_mem_properties(dev)) {}

device_memory_manager::~device_memory_manager() {}

vk::DeviceSize device_memory_manager::capacity() const {
	return block_size_ * blocks_.size() +
		   std::accumulate(separate_blocks_.begin(), separate_blocks_.end(), vk::DeviceSize(0u),
						   [](vk::DeviceSize sum, const device_memory_block& blk) { return sum + blk.size; });
}

} /* namespace graphics */
} /* namespace mce */
