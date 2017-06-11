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

/**
 * \file
 * Provides mathematical tool functions.
 */

namespace mce {
namespace util {

/// Determines the number of components in a glm vector.
template <typename>
struct vector_size {};

/// Determines the number of components in a glm vector.
template <typename T, glm::precision p>
struct vector_size<glm::tvec1<T, p>> {
	static constexpr int value = 1;
};

/// Determines the number of components in a glm vector.
template <typename T, glm::precision p>
struct vector_size<glm::tvec2<T, p>> {
	static constexpr int value = 2;
};

/// Determines the number of components in a glm vector.
template <typename T, glm::precision p>
struct vector_size<glm::tvec3<T, p>> {
	static constexpr int value = 3;
};

/// Determines the number of components in a glm vector.
template <typename T, glm::precision p>
struct vector_size<glm::tvec4<T, p>> {
	static constexpr int value = 4;
};

/// Returns the maximum value of all components a glm vector.
template <typename T, glm::precision p, template <typename, glm::precision> class Vector_Type>
T component_max(const Vector_Type<T, p>& v) {
	using std::max;
	T m = v[0];
	for(int i = 1; i < vector_size<Vector_Type<T, p>>::value; ++i) {
		m = max(m, v[i]);
	}
	return m;
}

/// Identity function to provide the overload for component_max for scalar numbers.
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_max(T v) {
	return v;
}

/// Returns the minimum value of all components a glm vector.
template <typename T, glm::precision p, template <typename, glm::precision> class Vector_Type>
T component_min(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(int i = 1; i < vector_size<Vector_Type<T, p>>::value; ++i) {
		m = min(m, v[i]);
	}
	return m;
}

/// Identity function to provide the overload for component_min for scalar numbers.
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_min(T v) {
	return v;
}

/// Returns the sum of all components a glm vector.
template <typename T, glm::precision p, template <typename, glm::precision> class Vector_Type>
T component_add(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(int i = 1; i < vector_size<Vector_Type<T, p>>::value; ++i) {
		m = m + v[i];
	}
	return m;
}

/// Identity function to provide the overload for component_add for scalar numbers.
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_add(T v) {
	return v;
}

/// Returns the product of all components a glm vector.
template <typename T, glm::precision p, template <typename, glm::precision> class Vector_Type>
T component_mul(const Vector_Type<T, p>& v) {
	T m = v[0];
	for(int i = 1; i < vector_size<Vector_Type<T, p>>::value; ++i) {
		m = m * v[i];
	}
	return m;
}

/// Identity function to provide the overload for component_mul for scalar numbers.
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
T component_mul(T v) {
	return v;
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_MATH_TOOLS_HPP_ */
