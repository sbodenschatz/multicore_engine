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

TEST(util_ring_chunk_placer_test, fill_up_buffer) {
	char buffer[256];
	ring_chunk_placer p(buffer, 256);
	char data[128];
	for(int j = 0; j < 16; j++) {
		memset(data, j + 1, 16);
		auto r = p.place_chunk(data, 16);
		ASSERT_TRUE(r);
	}
	char dummy = 17;
	auto r = p.place_chunk(&dummy, 1);
	ASSERT_FALSE(r);
	for(int j = 0; j < 16; j++)
		for(int i = 0; i < 16; ++i) ASSERT_EQ(j + 1, buffer[j * 16 + i]);
}

TEST(util_ring_chunk_placer_test, fill_up_reuse_one_buffer) {
	char buffer[256];
	ring_chunk_placer p(buffer, 256);
	char data[128];
	for(int j = 0; j < 16; j++) {
		memset(data, j + 1, 16);
		auto r = p.place_chunk(data, 16);
		ASSERT_TRUE(r);
	}
	char dummy = 17;
	auto r = p.place_chunk(&dummy, 1);
	ASSERT_FALSE(r);
	for(int j = 0; j < 16; j++)
		for(int i = 0; i < 16; ++i) ASSERT_EQ(j + 1, buffer[j * 16 + i]);

	p.free_to(1 * 16);
	memset(data, 17, 16);
	r = p.place_chunk(data, 16);
	ASSERT_FALSE(r);

	memset(data, 18, 15);
	r = p.place_chunk(data, 15);
	ASSERT_TRUE(r);

	dummy = 19;
	r = p.place_chunk(&dummy, 1);
	ASSERT_FALSE(r);
	for(int i = 0; i < 15; ++i) ASSERT_EQ(18, buffer[i]);
}
TEST(util_ring_chunk_placer_test, fill_up_reuse_two_buffers) {
	char buffer[256];
	ring_chunk_placer p(buffer, 256);
	char data[128];
	for(int j = 0; j < 16; j++) {
		memset(data, j + 1, 16);
		auto r = p.place_chunk(data, 16);
		ASSERT_TRUE(r);
	}
	char dummy = 17;
	auto r = p.place_chunk(&dummy, 1);
	ASSERT_FALSE(r);
	for(int j = 0; j < 16; j++)
		for(int i = 0; i < 16; ++i) ASSERT_EQ(j + 1, buffer[j * 16 + i]);

	p.free_to(1 * 16);
	memset(data, 17, 16);
	r = p.place_chunk(data, 16);
	ASSERT_FALSE(r);

	memset(data, 18, 15);
	r = p.place_chunk(data, 15);
	ASSERT_TRUE(r);
	for(int i = 0; i < 15; ++i) ASSERT_EQ(18, buffer[i]);

	p.free_to(2 * 16);
	memset(data, 19, 16);
	r = p.place_chunk(data, 16);
	ASSERT_TRUE(r);

	dummy = 20;
	r = p.place_chunk(&dummy, 1);
	ASSERT_FALSE(r);
	for(int i = 0; i < 16; ++i) ASSERT_EQ(19, buffer[i + 15]);
}

} // namespace util
} // namespace mce
