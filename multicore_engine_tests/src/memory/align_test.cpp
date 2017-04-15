/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/align_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <memory/align.hpp>

namespace mce {
namespace memory {

TEST(memory_align_test, align_success) {
	for(size_t alignment = 1; alignment <= 128; alignment *= 2) {
		for(size_t size = 1; size <= 256; size *= 2) {
			for(uintptr_t address = 0x10000; address < 0x10100; address++) {
				char* ptr_orig = reinterpret_cast<char*>(address);
				char* ptr = ptr_orig;
				void* vptr = ptr;
				size_t space = size + alignment;
				size_t space_orig = space;
				void* res = mce::memory::align(alignment, size, vptr, space);
				ptr = reinterpret_cast<char*>(vptr);
				uintptr_t aligned_address = reinterpret_cast<uintptr_t>(ptr);
				ASSERT_TRUE(res == ptr);
				ASSERT_TRUE(res != nullptr);
				ASSERT_TRUE(ptr + size <= ptr_orig + space_orig);
				ASSERT_TRUE(intptr_t(ptr - ptr_orig) < intptr_t(alignment));
				ASSERT_TRUE(intptr_t(ptr - ptr_orig) >= 0);
				ASSERT_TRUE(ptr >= ptr_orig);
				ASSERT_TRUE((aligned_address & (alignment - 1)) == 0);
				ASSERT_TRUE(space <= space_orig);
				ASSERT_TRUE(space + intptr_t(ptr - ptr_orig) == space_orig);
			}
		}
	}
}
TEST(memory_align_test, align_failure) {
	for(size_t alignment = 1; alignment <= 128; alignment *= 2) {
		for(size_t size = 1; size <= 256; size *= 2) {
			for(uintptr_t address = 0x10000; address < 0x10100; address++) {
				char* ptr_orig = reinterpret_cast<char*>(address);
				char* ptr = ptr_orig;
				void* vptr = ptr;
				size_t space = size;
				size_t space_orig = space;
				void* res = mce::memory::align(alignment, size, vptr, space);
				ptr = reinterpret_cast<char*>(vptr);
				if((address & (alignment - 1)) != 0) {
					ASSERT_TRUE(res == nullptr);
				} else {
					ASSERT_TRUE(res == ptr);
				}
				ASSERT_TRUE(ptr == ptr_orig);
				ASSERT_TRUE(space == space_orig);
			}
		}
	}
}

} // namespace memory
} // namespace mce
