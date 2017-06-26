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

TEST(util_ring_chunk_placer_test, wrapping) {
	char buffer[256];
	ring_chunk_placer p(buffer, 256);
	char data[128];

	memset(data, 1, 128);
	auto r = p.place_chunk(data, 128);
	ASSERT_TRUE(r);
	for(int i = 0; i < 128; ++i) ASSERT_EQ(1, static_cast<char*>(r)[i]);

	memset(data, 2, 64);
	r = p.place_chunk(data, 64);
	ASSERT_TRUE(r);
	for(int i = 0; i < 64; ++i) ASSERT_EQ(2, static_cast<char*>(r)[i]);

	p.free_to(128);

	memset(data, 3, 128);
	r = p.place_chunk(data, 128);
	ASSERT_FALSE(r);

	memset(data, 4, 127);
	r = p.place_chunk(data, 127);
	ASSERT_TRUE(r);
	ASSERT_EQ(buffer, r);
	for(int i = 0; i < 127; ++i) ASSERT_EQ(4, static_cast<char*>(r)[i]);

	p.free_to(192);

	memset(data, 5, 129);
	r = p.place_chunk(data, 129);
	ASSERT_TRUE(r);
	ASSERT_EQ(buffer + 127, r);
	for(int i = 0; i < 129; ++i) ASSERT_EQ(5, static_cast<char*>(r)[i]);
}

} // namespace util
} // namespace mce
