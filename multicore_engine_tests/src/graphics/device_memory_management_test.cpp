/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/device_memory_management.cpp
 * Copyright 2016-2018 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <iostream>
#include <mce/graphics/device_memory_handle.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/vk_mock_interface.hpp>

namespace mce {
namespace graphics {
namespace vk_mock_interface {
uint32_t get_memory_type_index(const vk::DeviceMemory& mem);
} /* namespace vk_mock_interface */

void check_allocs(const std::vector<device_memory_allocation>& allocs) {
	bool disjunct = true;
	for(size_t i = 0; i < allocs.size(); ++i) {
		for(size_t j = 0; j < i; ++j) {
			const auto& a = allocs[i];
			const auto& b = allocs[j];
			if(a.block_id == b.block_id) {
				auto a_end = a.internal_offset + a.internal_size;
				auto b_end = b.internal_offset + b.internal_size;
				disjunct = disjunct && (a.internal_offset >= b_end || b.internal_offset >= a_end);
			}
		}
	}
	ASSERT_TRUE(disjunct);
}

TEST(graphics_device_memory_manager_test, simple_allocate_stack_non_overlapping) {
	device_memory_manager mm(nullptr, 0x10000);
	int allocations = 0x1000;
	size_t alloc_size = 0x1000;
	size_t alignment = 0x10;
	std::vector<device_memory_allocation> allocs;
	for(int i = 0; i < allocations; ++i) {
		vk::MemoryRequirements mem_req;
		mem_req.size = alloc_size;
		mem_req.alignment = alignment;
		mem_req.memoryTypeBits = 0x1F;
		auto alloc = mm.allocate(mem_req, true);
		allocs.push_back(alloc);
		ASSERT_TRUE((alloc.aligned_offset & (alignment - 1)) == 0);
	}
	check_allocs(allocs);
	for(auto it = allocs.rbegin(); it != allocs.rend(); ++it) {
		mm.free(*it);
	}
}
TEST(graphics_device_memory_manager_test, simple_allocate_unordered_non_overlapping) {
	device_memory_manager mm(nullptr, 0x10000);
	size_t alloc_sizes[] = {0x4, 0x10, 0x8, 0x18, 0x100, 0x1000, 0x10000, 0x20000, 0x2000, 0x400};
	size_t alloc_sizes_count = sizeof(alloc_sizes) / sizeof(alloc_sizes[0]);
	size_t alloc_align[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40};
	size_t alloc_align_count = sizeof(alloc_align) / sizeof(alloc_align[0]);
	int iterations = 0x200;
	std::vector<device_memory_allocation> allocs;
	for(int i = 0; i < iterations; ++i) {
		vk::MemoryRequirements mem_req;
		mem_req.size = alloc_sizes[i % alloc_sizes_count];
		mem_req.alignment = alloc_align[i % alloc_align_count];
		mem_req.memoryTypeBits = 0x1F;
		auto alloc = mm.allocate(mem_req, true);
		allocs.push_back(alloc);
		ASSERT_TRUE((alloc.aligned_offset & (alloc_align[i % alloc_align_count] - 1)) == 0);
		check_allocs(allocs);
		if(i % 2 == 0 && allocs.size()) {
			auto it = allocs.begin() + i % allocs.size();
			mm.free(*it);
			allocs.erase(it);
		}
	}
	for(auto it = allocs.rbegin(); it != allocs.rend(); ++it) {
		mm.free(*it);
	}
}

TEST(graphics_device_memory_manager_test, type_constraints_valid) {
	device_memory_manager mm(nullptr, 0x10000);
	std::vector<device_memory_allocation> allocs;
	for(uint32_t type_req_bits = 1; type_req_bits < 0x20; ++type_req_bits) {
		vk::MemoryRequirements mem_req;
		mem_req.size = 0x200;
		mem_req.alignment = 0x10;
		mem_req.memoryTypeBits = type_req_bits;
		auto alloc = mm.allocate(mem_req, true, vk::MemoryPropertyFlags());
		uint64_t type_index = vk_mock_interface::get_memory_type_index(alloc.memory_object);
		ASSERT_TRUE((1 << type_index) & type_req_bits);
		allocs.push_back(alloc);
		ASSERT_TRUE((alloc.aligned_offset & 0xF) == 0);
		check_allocs(allocs);
	}
	for(auto it = allocs.rbegin(); it != allocs.rend(); ++it) {
		mm.free(*it);
	}
}

TEST(graphics_device_memory_manager_test, flag_constraints_valid) {
	device_memory_manager mm(nullptr, 0x10000);
	std::vector<device_memory_allocation> allocs;
	vk::MemoryPropertyFlags flags[] = {
			vk::MemoryPropertyFlags(),
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			vk::MemoryPropertyFlagBits::eHostVisible,
			vk::MemoryPropertyFlagBits::eHostCoherent,
			vk::MemoryPropertyFlagBits::eHostCached,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
					vk::MemoryPropertyFlagBits::eHostCached};
	uint32_t type_req_bits[] = {0x1F, 0x04, 0x18, 0x18, 0x10, 0x18, 0x10};
	static_assert(sizeof(flags) / sizeof(flags[0]) == sizeof(type_req_bits) / sizeof(type_req_bits[0]),
				  "Input and expected output must match in size.");

	for(size_t i = 1; i < sizeof(flags) / sizeof(flags[0]); ++i) {
		vk::MemoryRequirements mem_req;
		mem_req.size = 0x200;
		mem_req.alignment = 0x10;
		mem_req.memoryTypeBits = 0x1F;
		auto alloc = mm.allocate(mem_req, true, flags[i]);
		uint64_t type_index = vk_mock_interface::get_memory_type_index(alloc.memory_object);
		ASSERT_TRUE((1 << type_index) & type_req_bits[i]);
		allocs.push_back(alloc);
		ASSERT_TRUE((alloc.aligned_offset & 0xF) == 0);
		check_allocs(allocs);
	}
	for(auto it = allocs.rbegin(); it != allocs.rend(); ++it) {
		mm.free(*it);
	}
}

TEST(graphics_device_memory_manager_test, device_memory_handle_test) {
	device_memory_manager mm(nullptr, 0x10000);
	int allocations = 0x500;
	size_t alloc_size = 0x1000;
	size_t alignment = 0x10;
	std::vector<device_memory_handle> allocs;
	for(int i = 0; i < allocations; ++i) {
		vk::MemoryRequirements mem_req;
		mem_req.size = alloc_size;
		mem_req.alignment = alignment;
		mem_req.memoryTypeBits = 0x1F;
		auto alloc = mm.allocate(mem_req, true);
		allocs.push_back(make_device_memory_handle(mm, alloc));
		ASSERT_TRUE((alloc.aligned_offset & (alignment - 1)) == 0);
	}
	allocs.clear();

	mm.cleanup(2);
	ASSERT_TRUE(mm.capacity() == 0x20000);

	mm.cleanup();
	ASSERT_TRUE(mm.capacity() == 0);
}

} // namespace graphics
} // namespace mce
