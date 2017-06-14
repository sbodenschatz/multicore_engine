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
	device_memory_allocation allocate(const vk::MemoryRequirements&,
									  vk::MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal) {
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

TEST(graphics_destruction_queue_manager_test, sufficient_retention_handle_advance) {
	destruction_queue_manager_test::test_memory_manager mm;
	std::pair<int32_t, int32_t> p2;
	std::pair<int32_t, int32_t> p3;
	std::pair<int32_t, int32_t> p4;
	{
		destruction_queue_manager dqm(nullptr, 3);
		auto f0 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		dqm.advance();
		auto f1 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		dqm.advance();
		auto f2 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f2));
		dqm.advance();
		auto f3 = destruction_queue_manager_test::alloc_frame_data_handle(mm, dqm);
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_all(mm.status(), f0));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f1));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f2));
		ASSERT_TRUE(destruction_queue_manager_test::check_handle_none(mm.status(), f3));
		dqm.advance();
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

TEST(graphics_destruction_queue_manager_test, destruction_order_handle) {
	destruction_queue_manager_test::test_memory_manager mm;
	{
		destruction_queue_manager dqm(nullptr, 3);
		auto h0 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h1 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h2 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h3 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h4 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		dqm.enqueue(std::move(h4));
		dqm.enqueue(std::move(h3));
		dqm.enqueue(std::move(h2));
		dqm.enqueue(std::move(h1));
		dqm.enqueue(std::move(h0));
		dqm.cleanup_and_set_current(1);
		auto h5 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h6 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h7 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h8 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h9 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h10 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		dqm.enqueue(std::move(h5));
		dqm.enqueue(std::move(h6));
		dqm.enqueue(std::move(h7));
		dqm.enqueue(std::move(h8));
		dqm.enqueue(std::move(h9));
		dqm.cleanup_and_set_current(2);
		dqm.enqueue(std::move(h10));
		dqm.cleanup_and_set_current(0);
		ASSERT_EQ(4, mm.status().at(0));
		ASSERT_EQ(3, mm.status().at(1));
		ASSERT_EQ(2, mm.status().at(2));
		ASSERT_EQ(1, mm.status().at(3));
		ASSERT_EQ(0, mm.status().at(4));
	}
	ASSERT_EQ(5, mm.status().at(5));
	ASSERT_EQ(6, mm.status().at(6));
	ASSERT_EQ(7, mm.status().at(7));
	ASSERT_EQ(8, mm.status().at(8));
	ASSERT_EQ(9, mm.status().at(9));
	ASSERT_EQ(10, mm.status().at(10));
}

TEST(graphics_destruction_queue_manager_test, destruction_order_handle_advance) {
	destruction_queue_manager_test::test_memory_manager mm;
	{
		destruction_queue_manager dqm(nullptr, 3);
		auto h0 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h1 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h2 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h3 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h4 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		dqm.enqueue(std::move(h4));
		dqm.enqueue(std::move(h3));
		dqm.enqueue(std::move(h2));
		dqm.enqueue(std::move(h1));
		dqm.enqueue(std::move(h0));
		dqm.advance();
		auto h5 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h6 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h7 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h8 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h9 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		auto h10 = make_device_memory_handle(mm, mm.allocate(vk::MemoryRequirements()));
		dqm.enqueue(std::move(h5));
		dqm.enqueue(std::move(h6));
		dqm.enqueue(std::move(h7));
		dqm.enqueue(std::move(h8));
		dqm.enqueue(std::move(h9));
		dqm.advance();
		dqm.enqueue(std::move(h10));
		dqm.advance();
		ASSERT_EQ(4, mm.status().at(0));
		ASSERT_EQ(3, mm.status().at(1));
		ASSERT_EQ(2, mm.status().at(2));
		ASSERT_EQ(1, mm.status().at(3));
		ASSERT_EQ(0, mm.status().at(4));
	}
	ASSERT_EQ(5, mm.status().at(5));
	ASSERT_EQ(6, mm.status().at(6));
	ASSERT_EQ(7, mm.status().at(7));
	ASSERT_EQ(8, mm.status().at(8));
	ASSERT_EQ(9, mm.status().at(9));
	ASSERT_EQ(10, mm.status().at(10));
}

TEST(graphics_destruction_queue_manager_test, sufficient_retention_executor) {
	bool d0 = false;
	bool d1 = false;
	bool d2 = false;
	bool d3 = false;
	bool d4 = false;
	{
		destruction_queue_manager dqm(nullptr, 3);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d0 = true; }));
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d1 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		dqm.cleanup_and_set_current(1);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d2 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		ASSERT_FALSE(d2);
		dqm.cleanup_and_set_current(2);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d3 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		ASSERT_FALSE(d2);
		ASSERT_FALSE(d3);
		dqm.cleanup_and_set_current(0);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d4 = true; }));
		ASSERT_TRUE(d0);
		ASSERT_TRUE(d1);
		ASSERT_FALSE(d2);
		ASSERT_FALSE(d3);
		ASSERT_FALSE(d4);
		dqm.cleanup_and_set_current(1);
		ASSERT_TRUE(d2);
		ASSERT_FALSE(d3);
		ASSERT_FALSE(d4);
	}
	ASSERT_TRUE(d3);
	ASSERT_TRUE(d4);
}

TEST(graphics_destruction_queue_manager_test, sufficient_retention_executor_advance) {
	bool d0 = false;
	bool d1 = false;
	bool d2 = false;
	bool d3 = false;
	bool d4 = false;
	{
		destruction_queue_manager dqm(nullptr, 3);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d0 = true; }));
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d1 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d2 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		ASSERT_FALSE(d2);
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d3 = true; }));
		ASSERT_FALSE(d0);
		ASSERT_FALSE(d1);
		ASSERT_FALSE(d2);
		ASSERT_FALSE(d3);
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d4 = true; }));
		ASSERT_TRUE(d0);
		ASSERT_TRUE(d1);
		ASSERT_FALSE(d2);
		ASSERT_FALSE(d3);
		ASSERT_FALSE(d4);
		dqm.advance();
		ASSERT_TRUE(d2);
		ASSERT_FALSE(d3);
		ASSERT_FALSE(d4);
	}
	ASSERT_TRUE(d3);
	ASSERT_TRUE(d4);
}

TEST(graphics_destruction_queue_manager_test, destruction_order_executor) {
	int index = 0;
	int d0 = -1;
	int d1 = -1;
	int d2 = -1;
	int d3 = -1;
	int d4 = -1;
	{
		destruction_queue_manager dqm(nullptr, 3);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d0 = index++; }));
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d1 = index++; }));
		dqm.cleanup_and_set_current(1);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d2 = index++; }));
		dqm.cleanup_and_set_current(2);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d3 = index++; }));
		dqm.cleanup_and_set_current(0);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d4 = index++; }));
		ASSERT_EQ(0, d0);
		ASSERT_EQ(1, d1);
		dqm.cleanup_and_set_current(1);
		ASSERT_EQ(2, d2);
	}
	ASSERT_EQ(3, d3);
	ASSERT_EQ(4, d4);
}

TEST(graphics_destruction_queue_manager_test, destruction_order_executor_advance) {
	int index = 0;
	int d0 = -1;
	int d1 = -1;
	int d2 = -1;
	int d3 = -1;
	int d4 = -1;
	{
		destruction_queue_manager dqm(nullptr, 3);
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d0 = index++; }));
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d1 = index++; }));
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d2 = index++; }));
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d3 = index++; }));
		dqm.advance();
		dqm.enqueue(destruction_queue_manager::make_executor([&]() { d4 = index++; }));
		ASSERT_EQ(0, d0);
		ASSERT_EQ(1, d1);
		dqm.advance();
		ASSERT_EQ(2, d2);
	}
	ASSERT_EQ(3, d3);
	ASSERT_EQ(4, d4);
}

TEST(graphics_destruction_queue_manager_test, destruction_locking) {
	destruction_queue_manager dqm(nullptr, 3);
	bool res = false;
	dqm.enqueue(destruction_queue_manager::make_executor(
			[&]() { dqm.enqueue(destruction_queue_manager::make_executor([&]() { res = true; })); }));
	for(int i = 0; i < 9; ++i) dqm.advance();
	ASSERT_TRUE(res);
}

} // namespace graphics
} // namespace mce
