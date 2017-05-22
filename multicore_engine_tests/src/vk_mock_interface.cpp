/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/include/graphics/vk_mock_interface.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/unique_handle.hpp>
#include <mce/graphics/vk_mock_interface.hpp>
#include <mce/graphics/window.hpp>
#include <mutex>
#include <unordered_map>

namespace mce {
namespace graphics {
namespace vk_mock_interface {

bool is_mocked() {
	return true;
}

static std::unordered_map<uint32_t, uint32_t> mock_alloc_ids;
static std::mutex mock_alloc_id_mutex;

unique_handle<vk::DeviceMemory, true> allocate_memory(mce::graphics::device*, vk::MemoryAllocateInfo& ai) {
	std::lock_guard<std::mutex> lock(mock_alloc_id_mutex);
	return unique_handle<vk::DeviceMemory, true>(
			vk::DeviceMemory(VkDeviceMemory((1ull << 63) | (uint64_t(ai.memoryTypeIndex) << 30) |
											uint64_t(mock_alloc_ids[ai.memoryTypeIndex]++))),
			[](vk::DeviceMemory&, const vk::Optional<const vk::AllocationCallbacks>&) {});
}

uint32_t get_memory_type_index(const vk::DeviceMemory& mem) {
	VkDeviceMemory mem_id = mem;
	return uint32_t((0x7FFFFFFFFFFFFFFF & uint64_t(mem_id)) >> 30);
}

vk::PhysicalDeviceMemoryProperties get_physical_dev_mem_properties(mce::graphics::device*) {
	vk::PhysicalDeviceMemoryProperties mem_props;
	mem_props.memoryHeapCount = 2;
	mem_props.memoryHeaps[0].flags = vk::MemoryHeapFlagBits::eDeviceLocal;
	mem_props.memoryHeaps[0].size = 2147483648;
	mem_props.memoryHeaps[1].flags = vk::MemoryHeapFlags();
	mem_props.memoryHeaps[1].size = 34252783616;

	mem_props.memoryTypeCount = 5;
	mem_props.memoryTypes[0].propertyFlags = vk::MemoryPropertyFlags();
	mem_props.memoryTypes[0].heapIndex = 1;
	mem_props.memoryTypes[1].propertyFlags = vk::MemoryPropertyFlags();
	mem_props.memoryTypes[1].heapIndex = 1;
	mem_props.memoryTypes[2].propertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mem_props.memoryTypes[2].heapIndex = 0;
	mem_props.memoryTypes[3].propertyFlags =
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	mem_props.memoryTypes[3].heapIndex = 1;
	mem_props.memoryTypes[4].propertyFlags = vk::MemoryPropertyFlagBits::eHostVisible |
											 vk::MemoryPropertyFlagBits::eHostCoherent |
											 vk::MemoryPropertyFlagBits::eHostCached;
	mem_props.memoryTypes[4].heapIndex = 1;

	return mem_props;
}

} /* namespace vk_mock_interface */
} /* namespace graphics */
} /* namespace mce */
