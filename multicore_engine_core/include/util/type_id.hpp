/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/type_id.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_TYPE_ID_HPP_
#define UTIL_TYPE_ID_HPP_

/**
 * \file
 * Provides generic type identification.
 */

#include <atomic>

namespace mce {
namespace util {

/// The type used for type ids generated by type_id.
typedef unsigned long long type_id_t;

/// Provides a way to assign unique ids types.
/**
 * The ids generated are dependent on the order in which the id member function is first called for a given
 * type. Therefore they should not be used across different binaries, executions or hosts.
 * The ids can be organized into groups (named id namespaces) that are identified by a tag type.
 * The default is the global namespace signified by void.
 */
template <typename ID_Namespace>
class type_id {
public:
	/// Returns a unique id for the given type T.
	template <typename T>
	static type_id_t id() {
		static type_id_t id_ = next_id++;
		return id_;
	}

private:
	static std::atomic<type_id_t> next_id;
};

template <typename ID_Namespace>
std::atomic<type_id_t> type_id<ID_Namespace>::next_id = {1};

} // namespace util
} // namespace mce

#endif /* UTIL_TYPE_ID_HPP_ */
