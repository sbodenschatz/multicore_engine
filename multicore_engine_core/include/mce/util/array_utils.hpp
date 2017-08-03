/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/array_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_ARRAY_UTILS_HPP_
#define MCE_UTIL_ARRAY_UTILS_HPP_

#include <array>

namespace mce {
namespace util {

template <typename... T>
std::array<std::common_type_t<T...>, sizeof...(T)> make_array(T&&... args) {
	return {std::forward<T>(args)...};
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_ARRAY_UTILS_HPP_ */
