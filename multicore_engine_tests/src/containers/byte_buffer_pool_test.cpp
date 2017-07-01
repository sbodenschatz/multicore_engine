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

} // namespace containers
} // namespace mce
