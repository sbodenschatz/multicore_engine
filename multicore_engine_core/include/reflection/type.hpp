/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/type.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef REFLECTION_TYPE_HPP_
#define REFLECTION_TYPE_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <numeric>
#include <sstream>
#include <string>
#include <util/string_tools.hpp>
#include <vector>

namespace mce {
namespace reflection {

/// Represents the type of a value.
enum class type_t {
	/// The type is not known or not supported.
	unknown,
	/// The value is a scalar integer.
	int_1,
	/// The value is a 2-dimensional vector of integers.
	int_2,
	/// The value is a 3-dimensional vector of integers.
	int_3,
	/// The value is a 4-dimensional vector of integers.
	int_4,
	/// The value is a scalar float.
	float_1,
	/// The value is a 2-dimensional vector of floats.
	float_2,
	/// The value is a 3-dimensional vector of floats.
	float_3,
	/// The value is a 4-dimensional vector of floats.
	float_4,
	/// The value is a quaternion.
	quaternion,
	/// The value is a textual string.
	string,
	/// The value is a list of textual strings.
	string_list,
};

/// Allows conversion of C++ types to the appropriate type_t member value using specializations.
template <typename T>
struct type_info {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::unknown;
	typedef std::false_type known_type;
};

/// Specialization of type_info for int.
template <>
struct type_info<int> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_1;
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec2.
template <>
struct type_info<glm::ivec2> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_2;
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec3.
template <>
struct type_info<glm::ivec3> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_3;
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec4.
template <>
struct type_info<glm::ivec4> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_4;
	typedef std::true_type known_type;
};

/// Specialization of type_info for float.
template <>
struct type_info<float> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_1;
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec2.
template <>
struct type_info<glm::vec2> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_2;
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec3.
template <>
struct type_info<glm::vec3> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_3;
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec4.
template <>
struct type_info<glm::vec4> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_4;
	typedef std::true_type known_type;
};

/// Specialization of type_info for quat.
template <>
struct type_info<glm::quat> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::quaternion;
	typedef std::true_type known_type;
};

/// Specialization of type_info for string.
template <>
struct type_info<std::string> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::string;
	typedef std::true_type known_type;
};

/// Specialization of type_info for vector<string>.
template <>
struct type_info<std::vector<std::string>> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::string_list;
	typedef std::true_type known_type;
};

template <typename T>
struct type_parser {
	static bool from_string(const std::string& s, T& t) {
		std::istringstream stream(s);
		stream >> t;
		return bool(stream);
	}
	static std::string to_string(const T& t) {
		std::ostringstream stream;
		stream << t;
		return stream.str();
	}
};

template <>
struct type_parser<glm::vec2> {
	static bool from_string(const std::string& s, glm::vec2& t) {
		std::istringstream stream(s);
		float x, y;
		x = y = 0.0f;
		stream >> x >> y;
		t = glm::vec2(x, y);
		return bool(stream);
	}
	static std::string to_string(const glm::vec2& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y;
		return stream.str();
	}
};
template <>
struct type_parser<glm::vec3> {
	static bool from_string(const std::string& s, glm::vec3& t) {
		std::istringstream stream(s);
		float x, y, z;
		x = y = z = 0.0f;
		stream >> x >> y >> z;
		t = glm::vec3(x, y, z);
		return bool(stream);
	}
	static std::string to_string(const glm::vec3& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z;
		return stream.str();
	}
};
template <>
struct type_parser<glm::vec4> {
	static bool from_string(const std::string& s, glm::vec4& t) {
		std::istringstream stream(s);
		float x, y, z, w;
		x = y = z = w = 0.0f;
		stream >> x >> y >> z >> w;
		t = glm::vec4(x, y, z, w);
		return bool(stream);
	}
	static std::string to_string(const glm::vec4& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

template <>
struct type_parser<glm::quat> {
	static bool from_string(const std::string& s, glm::quat& t) {
		std::istringstream stream(s);
		float x, y, z, w;
		x = y = z = w = 0.0f;
		stream >> x >> y >> z >> w;
		t = glm::quat(x, y, z, w);
		return bool(stream);
	}
	static std::string to_string(const glm::quat& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

template <>
struct type_parser<glm::ivec2> {
	static bool from_string(const std::string& s, glm::ivec2& t) {
		std::istringstream stream(s);
		int x, y;
		x = y = 0;
		stream >> x >> y;
		t = glm::ivec2(x, y);
		return bool(stream);
	}
	static std::string to_string(const glm::ivec2& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y;
		return stream.str();
	}
};
template <>
struct type_parser<glm::ivec3> {
	static bool from_string(const std::string& s, glm::ivec3& t) {
		std::istringstream stream(s);
		int x, y, z;
		x = y = z = 0;
		stream >> x >> y >> z;
		t = glm::ivec3(x, y, z);
		return bool(stream);
	}
	static std::string to_string(const glm::ivec3& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z;
		return stream.str();
	}
};
template <>
struct type_parser<glm::ivec4> {
	static bool from_string(const std::string& s, glm::ivec4& t) {
		std::istringstream stream(s);
		int x, y, z, w;
		x = y = z = w = 0;
		stream >> x >> y >> z >> w;
		t = glm::ivec4(x, y, z, w);
		return bool(stream);
	}
	static std::string to_string(const glm::ivec4& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

template <>
struct type_parser<std::string> {
	static bool from_string(const std::string& s, std::string& t) {
		t = s;
		return true;
	}
	static std::string to_string(const std::string& t) {
		return t;
	}
};

template <>
struct type_parser<std::vector<std::string>> {
	static std::string delimiter;
	static bool from_string(const std::string& s, std::vector<std::string>& t) {
		t.clear();
		util::split_iterate(s, delimiter, [&t](boost::string_view e) { t.emplace_back(e); });
		return true;
	}
	static std::string to_string(const std::vector<std::string>& t) {
		using namespace std::literals;
		return std::accumulate(t.begin(), t.end(), ""s, [](const auto& x, const auto& y) {
			return x + (x.empty() ? ""s : delimiter) + y;
		});
	}
};


} // namespace reflection
} // namespace mce

#endif /* REFLECTION_TYPE_HPP_ */
