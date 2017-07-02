/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/byte_buffer_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <cstring>
#include <gtest.hpp>
#include <mce/containers/byte_buffer_pool.hpp>

namespace mce {
namespace containers {

TEST(containers_byte_buffer_pool_test, allocate_multiple_buffers) {
	byte_buffer_pool p;
	std::vector<pooled_byte_buffer_ptr> buffs;
	for(size_t i = 0; i < 256; ++i) {
		auto b = p.allocate_buffer(1024);
		buffs.push_back(b);
		memset(b.data(), i % 128, 1024);
	}

	for(size_t i = 0; i < buffs.size(); ++i) {
		ASSERT_TRUE(std::all_of(buffs[i].data(), buffs[i].data() + buffs[i].size(),
								[i](auto c) { return size_t(c) == i % 128; }));
	}
}

TEST(containers_byte_buffer_pool_test, pool_buffers_reuse) {
	byte_buffer_pool p;
	std::vector<pooled_byte_buffer_ptr> buffs;
	auto b = p.allocate_buffer(1024);
	buffs.push_back(b);
	auto cap = p.capacity();
	size_t i = 0;
	while(cap == p.capacity()) {
		auto b = p.allocate_buffer(1024);
		buffs.push_back(b);
		memset(b, i % 128, b.size());
		ASSERT_TRUE(std::all_of(b.begin(), b.end(), [i](auto c) { return size_t(c) == i % 128; }));
		++i;
	}
	cap = p.capacity();
	buffs.clear();
	for(; i > 0; --i) {
		auto b = p.allocate_buffer(512);
		buffs.push_back(b);
		memset(b, i % 128, b.size());
		ASSERT_TRUE(std::all_of(b.begin(), b.end(), [i](auto c) { return size_t(c) == i % 128; }));
		auto b2 = p.allocate_buffer(512);
		buffs.push_back(b2);
		memset(b2, i % 128, b2.size());
		ASSERT_TRUE(std::all_of(b2.begin(), b2.end(), [i](auto c) { return size_t(c) == i % 128; }));
	}
	ASSERT_EQ(cap, p.capacity());
}

} // namespace containers
} // namespace mce
