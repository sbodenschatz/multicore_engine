/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/type.hpp
 * Copyright 2015 by Stefan Bodenschatz
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

enum class type_t {
	unknown,
	int_1,
	int_2,
	int_3,
	int_4,
	float_1,
	float_2,
	float_3,
	float_4,
	quaternion,
	string,
	string_list,
};

template <typename T>
struct type_info {
	static const type_t type = type_t::unknown;
};

template <>
struct type_info<int> {
	static const type_t type = type_t::int_1;
};
template <>
struct type_info<glm::ivec2> {
	static const type_t type = type_t::int_2;
};
template <>
struct type_info<glm::ivec3> {
	static const type_t type = type_t::int_3;
};
template <>
struct type_info<glm::ivec4> {
	static const type_t type = type_t::int_4;
};

template <>
struct type_info<float> {
	static const type_t type = type_t::float_1;
};

template <>
struct type_info<glm::vec2> {
	static const type_t type = type_t::float_2;
};

template <>
struct type_info<glm::vec3> {
	static const type_t type = type_t::float_3;
};

template <>
struct type_info<glm::vec4> {
	static const type_t type = type_t::float_4;
};

template <>
struct type_info<glm::quat> {
	static const type_t type = type_t::quaternion;
};

template <>
struct type_info<std::string> {
	static const type_t type = type_t::string;
};

template <>
struct type_info<std::vector<std::string>> {
	static const type_t type = type_t::string_list;
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

std::string type_parser<std::vector<std::string>>::delimiter = ";";

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_TYPE_HPP_ */
