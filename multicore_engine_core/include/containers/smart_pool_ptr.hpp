/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/smart_pool_ptr.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SMART_POOL_PTR_HPP_
#define CONTAINERS_SMART_POOL_PTR_HPP_

namespace mce {
namespace containers {

template <typename T>
class smart_pool_ptr {
	T* object;
	detail::smart_object_pool_block_interface<T>* block;

	template <size_t block_size>
	friend class smart_object_pool<T, block_size>;

	smart_pool_ptr(T* object, detail::smart_object_pool_block_interface<T>* block) noexcept : object{object},
																							  block{block} {}

public:
};

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SMART_POOL_PTR_HPP_ */
