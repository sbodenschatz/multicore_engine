/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/align_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <memory/align.hpp>

namespace mce {
namespace memory {

BOOST_AUTO_TEST_SUITE(memory)
BOOST_AUTO_TEST_SUITE(align_test)

BOOST_AUTO_TEST_CASE(align_success) {
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
				BOOST_CHECK(res == ptr);
				BOOST_CHECK(res != nullptr);
				BOOST_CHECK(ptr + size <= ptr_orig + space_orig);
				BOOST_CHECK(intptr_t(ptr - ptr_orig) < intptr_t(alignment));
				BOOST_CHECK(intptr_t(ptr - ptr_orig) >= 0);
				BOOST_CHECK(ptr >= ptr_orig);
				BOOST_CHECK((aligned_address & (alignment - 1)) == 0);
				BOOST_CHECK(space <= space_orig);
				BOOST_CHECK(space + intptr_t(ptr - ptr_orig) == space_orig);
			}
		}
	}
}
BOOST_AUTO_TEST_CASE(align_failure) {
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
					BOOST_CHECK(res == nullptr);
				} else {
					BOOST_CHECK(res == ptr);
				}
				BOOST_CHECK(ptr == ptr_orig);
				BOOST_CHECK(space == space_orig);
			}
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace memory
} // namespace mce
