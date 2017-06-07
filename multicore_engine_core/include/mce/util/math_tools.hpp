/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/math_tools.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_MATH_TOOLS_HPP_
#define MCE_UTIL_MATH_TOOLS_HPP_

#include <algorithm>
#include <glm/glm.hpp>
#include <type_traits>

namespace mce {
namespace util {

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
constexpr auto vector_size(const Vector_Type<T, p>& v) {
#ifdef GLM_FORCE_SIZE_FUNC
	return v.size();
#else
	return v.length();
#endif
}

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
T component_max(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(decltype(vector_size(v)) i = 1; i < vector_size(v); ++i) {
		m = max(m, v[i]);
	}
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_max(T v) {
	return v;
}

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
T component_min(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(decltype(vector_size(v)) i = 1; i < vector_size(v); ++i) {
		m = min(m, v[i]);
	}
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_min(T v) {
	return v;
}

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
T component_add(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(decltype(vector_size(v)) i = 1; i < vector_size(v); ++i) {
		m = m + v[i];
	}
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_add(T v) {
	return v;
}

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
T component_mul(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(decltype(vector_size(v)) i = 1; i < vector_size(v); ++i) {
		m = m * v[i];
	}
}

template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_mul(T v) {
	return v;
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_MATH_TOOLS_HPP_ */
