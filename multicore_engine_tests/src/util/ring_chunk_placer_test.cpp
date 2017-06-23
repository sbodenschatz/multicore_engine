/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/ring_chunk_placer_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/util/ring_chunk_placer.hpp>

namespace mce {
namespace util {

TEST(util_ring_chunk_placer_test, place_single_chunk_in_empty_buffer) {
	char buffer[128];
	ring_chunk_placer p(buffer, 128);
	char data[128];
	memset(data, 1, 16);
	p.place_chunk(data, 16);
	for(int i = 0; i < 16; ++i) ASSERT_EQ(1, buffer[i]);
}

} // namespace util
} // namespace mce
