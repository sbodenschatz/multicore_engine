/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/type_id.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_TYPE_ID_HPP_
#define UTIL_TYPE_ID_HPP_

#include <atomic>

namespace mce {
namespace util {

/// Provides a way to assign unique ids types.
/**
 * The ids generated are dependent on the order in which the id member function is first called for a given
 * type. Therefore they should not be used across different binaries, executions or hosts.
 */
class type_id {
public:
	/// The type used for type_ids.
	typedef unsigned long long type_id_t;
	/// Returns a unique id for the given type T.
	template <typename T>
	static type_id_t id() {
		static type_id_t id_ = next_id++;
		return id_;
	}

private:
	static std::atomic<type_id_t> next_id;
};

} // namespace util
} // namespace mce

#endif /* UTIL_TYPE_ID_HPP_ */
