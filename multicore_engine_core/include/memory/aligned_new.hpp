/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/memory/aligned_new.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef MEMORY_ALIGNED_NEW_HPP_
#define MEMORY_ALIGNED_NEW_HPP_

#include "align.hpp"
#include <cassert>

#define ALIGNED_NEW_AND_DELETE(TYPE)                                                                               \
	static void* operator new(std::size_t count) {                                                                  \
		size_t alignment = alignof(TYPE);                                                         \
		size_t space = count + alignment + sizeof(void*);                                                    \
		void* orig = ::operator new(space);                                                                  \
		void* tmp = reinterpret_cast<char*>(orig) + sizeof(void*);                                           \
		void* res = mce::memory::align(alignment, count, tmp, space);                                                \
		assert(res);                                                                                         \
		if(!res) throw std::bad_alloc();                                                                     \
		*(reinterpret_cast<void**>(res) - 1) = orig;                                                         \
		return res;                                                                                          \
	\
}                                                                                                     \
	\
static void* operator new[](std::size_t count) {                                                                      \
		size_t alignment = alignof(TYPE);                                                         \
		size_t space = count + alignment + sizeof(void*);                                                    \
		void* orig = ::operator new[](space);                                                                \
		void* tmp = reinterpret_cast<char*>(orig) + sizeof(void*);                                           \
		void* res = mce::memory::align(alignment, count, tmp, space);                                                \
		assert(res);                                                                                         \
		if(!res) throw std::bad_alloc();                                                                     \
		*(reinterpret_cast<void**>(res) - 1) = orig;                                                         \
		return res;                                                                                          \
	\
}                                                                                                     \
	\
void                                                                                                  \
	operator delete(void* ptr) {                                                                             \
		::operator delete(*(reinterpret_cast<void**>(ptr) - 1));                                             \
	\
}                                                                                                     \
	\
void                                                                                                  \
	operator delete[](void* ptr) {                                                                           \
		::operator delete[](*(reinterpret_cast<void**>(ptr) - 1));                                           \
	\
}

#endif /* MEMORY_ALIGNED_NEW_HPP_ */
