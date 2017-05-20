/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/type.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef REFLECTION_TYPE_HPP_
#define REFLECTION_TYPE_HPP_

/**
 * \file
 * Defines type-related helper code for the property reflection system.
 */

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <numeric>
#include <sstream>
#include <string>
#include <type_traits>
#include <util/string_tools.hpp>
#include <vector>

namespace mce {
namespace bstream {
class ibstream;
class obstream;
} // namespace bstream

namespace entity {
class entity_reference;
} // namespace entity

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
	/// The value is a reference to a named entity.
	entity_reference
};
/// Deserializes the value type representation from the bstream.
bstream::ibstream& operator>>(bstream::ibstream& ibs, type_t& value);
/// Serializes the value type representation to the bstream.
bstream::obstream& operator<<(bstream::obstream& obs, type_t value);

/// Allows conversion of C++ types to the appropriate type_t member value using specializations.
template <typename T>
struct type_info {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::unknown;
	/// Specifies that this is the general case where the type is not known.
	typedef std::false_type known_type;
};

/// Specialization of type_info for int.
template <>
struct type_info<int> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_1;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec2.
template <>
struct type_info<glm::ivec2> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_2;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec3.
template <>
struct type_info<glm::ivec3> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_3;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for ivec4.
template <>
struct type_info<glm::ivec4> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::int_4;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for float.
template <>
struct type_info<float> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_1;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec2.
template <>
struct type_info<glm::vec2> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_2;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec3.
template <>
struct type_info<glm::vec3> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_3;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for vec4.
template <>
struct type_info<glm::vec4> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::float_4;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for quat.
template <>
struct type_info<glm::quat> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::quaternion;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for string.
template <>
struct type_info<std::string> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::string;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for vector<string>.
template <>
struct type_info<std::vector<std::string>> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::string_list;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Specialization of type_info for entity::entity_reference.
template <>
struct type_info<entity::entity_reference> {
	/// The value representing the type_t representation of T.
	static const type_t type = type_t::entity_reference;
	/// Specifies that this is a specialized case where the type is known.
	typedef std::true_type known_type;
};

/// Allows formating and parsing of C++ types using specializations and stream IO operators.
template <typename T>
struct type_parser {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, T& t) {
		std::istringstream stream(s.to_string());
		stream >> t;
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const T& t) {
		std::ostringstream stream;
		stream << t;
		return stream.str();
	}
};

/// Specialization of type_parser for glm::vec2.
template <>
struct type_parser<glm::vec2> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::vec2& t) {
		std::istringstream stream(s.to_string());
		float x, y;
		x = y = 0.0f;
		stream >> x >> y;
		t = glm::vec2(x, y);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::vec2& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y;
		return stream.str();
	}
};
/// Specialization of type_parser for glm::vec3.
template <>
struct type_parser<glm::vec3> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::vec3& t) {
		std::istringstream stream(s.to_string());
		float x, y, z;
		x = y = z = 0.0f;
		stream >> x >> y >> z;
		t = glm::vec3(x, y, z);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::vec3& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z;
		return stream.str();
	}
};
/// Specialization of type_parser for glm::vec4.
template <>
struct type_parser<glm::vec4> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::vec4& t) {
		std::istringstream stream(s.to_string());
		float x, y, z, w;
		x = y = z = w = 0.0f;
		stream >> x >> y >> z >> w;
		t = glm::vec4(x, y, z, w);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::vec4& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

/// Specialization of type_parser for glm::quat.
template <>
struct type_parser<glm::quat> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::quat& t) {
		std::istringstream stream(s.to_string());
		float x, y, z, w;
		x = y = z = w = 0.0f;
		stream >> x >> y >> z >> w;
		t = glm::quat(x, y, z, w);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::quat& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

/// Specialization of type_parser for glm::ivec2.
template <>
struct type_parser<glm::ivec2> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::ivec2& t) {
		std::istringstream stream(s.to_string());
		int x, y;
		x = y = 0;
		stream >> x >> y;
		t = glm::ivec2(x, y);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::ivec2& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y;
		return stream.str();
	}
};
/// Specialization of type_parser for glm::ivec3.
template <>
struct type_parser<glm::ivec3> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::ivec3& t) {
		std::istringstream stream(s.to_string());
		int x, y, z;
		x = y = z = 0;
		stream >> x >> y >> z;
		t = glm::ivec3(x, y, z);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::ivec3& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z;
		return stream.str();
	}
};
/// Specialization of type_parser for glm::ivec4.
template <>
struct type_parser<glm::ivec4> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, glm::ivec4& t) {
		std::istringstream stream(s.to_string());
		int x, y, z, w;
		x = y = z = w = 0;
		stream >> x >> y >> z >> w;
		t = glm::ivec4(x, y, z, w);
		return bool(stream);
	}
	/// Formats t into a string.
	static std::string to_string(const glm::ivec4& t) {
		std::ostringstream stream;
		stream << t.x << " " << t.y << " " << t.z << " " << t.w;
		return stream.str();
	}
};

/// Specialization of type_parser for std::string.
template <>
struct type_parser<std::string> {
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, std::string& t) {
		t = s.to_string();
		return true;
	}
	/// Formats t into a string.
	static std::string to_string(const std::string& t) {
		return t;
	}
};

/// Specialization of type_parser for std::vector<std::string>.
template <>
struct type_parser<std::vector<std::string>> {
	/// The delimiter used to separate list elements (by default ";").
	static std::string delimiter;
	/// Parses s into t.
	static bool from_string(const boost::string_view& s, std::vector<std::string>& t) {
		t.clear();
		util::split_iterate(s, delimiter, [&t](boost::string_view e) { t.emplace_back(e); });
		return true;
	}
	/// Formats t into a string.
	static std::string to_string(const std::vector<std::string>& t) {
		using namespace std::literals;
		return std::accumulate(t.begin(), t.end(), ""s, [](const auto& x, const auto& y) {
			return x + (x.empty() ? ""s : delimiter) + y;
		});
	}
};

} // namespace reflection
} // namespace mce

#include <entity/entity_reference.hpp>

namespace mce {
namespace reflection {

/// Specialization of type_parser for entity::entity_reference.
template <>
struct type_parser<entity::entity_reference> {
	/// Parses s into t.
	static bool from_string(const boost::string_view&, entity::entity_reference&) {
		/// TODO: Check if this operation can be supported in the future (an entity_manager would be required
		/// here, reducing generality of the property).
		return false;
	}
	/// Formats t into a string.
	static std::string to_string(const entity::entity_reference& t) {
		return t.referenced_entity_name();
	}
};

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_TYPE_HPP_ */
