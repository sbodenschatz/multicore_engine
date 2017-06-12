/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/destruction_queue_manager_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <gtest.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <vector>

namespace mce {
namespace graphics {
namespace destruction_queue_manager_test {

class test_memory_manager : public device_memory_manager_interface {
	std::vector<int> destroyed_map;
	int deletion_index = 0;

public:
	void free(const device_memory_allocation& allocation) {
		EXPECT_GT(destroyed_map.size(), allocation.block_id);
		destroyed_map[allocation.block_id] = deletion_index++;
	}
	device_memory_allocation
	allocate(const vk::MemoryRequirements& ,
			 vk::MemoryPropertyFlags  = vk::MemoryPropertyFlagBits::eDeviceLocal) {
		device_memory_allocation a;
		a.block_id = int32_t(destroyed_map.size());
		destroyed_map.push_back(-1);
		return a;
	}
	const std::vector<int>& status() {
		return destroyed_map;
	}
};

std::pair<int32_t, int32_t> alloc_frame_data_handle(test_memory_manager& mm, destruction_queue_manager& dqm) {
	std::pair<int32_t, int32_t> range;
	auto h1 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
	auto h2 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
	auto h3 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
	auto h4 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
	range.first = h1.allocation().block_id;
	range.second = h4.allocation().block_id;
	dqm.enqueue(std::move(h1));
	dqm.enqueue(std::move(h2));
	dqm.enqueue(std::move(h3));
	dqm.enqueue(std::move(h4));
	return range;
}

bool check_handle_all(const std::vector<int>& status, std::pair<int32_t, int32_t> range) {
	return std::all_of(status.begin() + range.first, status.begin() + range.second + 1,
					   [](int e) { return e >= 0; });
}
bool check_handle_none(const std::vector<int>& status, std::pair<int32_t, int32_t> range) {
	return std::none_of(status.begin() + range.first, status.begin() + range.second + 1,
						[](int e) { return e >= 0; });
}

} // namespace destruction_queue_manager_test

TEST(graphics_destruction_queue_manager_test, sufficient_retention_handle) {
	destruction_queue_manager_test::test_memory_manager mm;
	std::pair<int32_t, int32_t> p2;
	std::pair<int32_t, int32_t> p3;
	std::pair<int32_t, int32_t> p4;
	{
		destruction_queue_manager dqm(nullptr, 3);
		auto f0 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		dqm.cleanup_and_set_current(1);
		auto f1 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		dqm.cleanup_and_set_current(2);
		auto f2 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f2));
		dqm.cleanup_and_set_current(0);
		auto f3 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f2));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f3));
		dqm.cleanup_and_set_current(1);
		auto f4 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), f1));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f2));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f3));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f4));
		p2 = f2;
		p3 = f3;
		p4 = f4;
	}
	ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), p2));
	ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), p3));
	ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), p4));
}

} // namespace graphics
} // namespace mce
