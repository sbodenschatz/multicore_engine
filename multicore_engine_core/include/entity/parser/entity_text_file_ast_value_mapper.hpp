/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast_value_map.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_

#include "entity_text_file_ast.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <type_traits>
#include <vector>

namespace mce {
namespace entity {
namespace ast {

template <typename AST_Value, typename Target, typename Dummy = void>
struct ast_value_mapper {
	typedef void error;
};

template <typename T>
struct ast_value_mapper<long long, T, std::enable_if_t<std::is_arithmetic<T>::value>> {
	static void convert(const long long& ast_val, T& val) {
		val = T(ast_val);
	}
};

template <typename T>
struct ast_value_mapper<float, T, std::enable_if_t<std::is_floating_point<T>::value>> {
	static void convert(const ast::float_node& ast_val, T& val) {
		val = T(ast_val);
	}
};

template <>
struct ast_value_mapper<std::string, std::string> {
	static void convert(const std::string& ast_val, std::string& val) {
		val = ast_val;
	}
};

template <>
struct ast_value_mapper<string_list, std::vector<std::string>> {
	static void convert(const string_list& ast_val, std::vector<std::string>& val) {
		val = ast_val;
	}
};

template <>
struct ast_value_mapper<float_list, glm::vec2> {
	static void convert(const float_list& ast_val, glm::vec2& val) {
		val = glm::vec2();
		for(unsigned int i = 0; i < 2 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

template <>
struct ast_value_mapper<float_list, glm::vec3> {
	static void convert(const float_list& ast_val, glm::vec3& val) {
		val = glm::vec3();
		for(unsigned int i = 0; i < 3 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

template <>
struct ast_value_mapper<float_list, glm::vec4> {
	static void convert(const float_list& ast_val, glm::vec4& val) {
		val = glm::vec4();
		for(unsigned int i = 0; i < 4 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec2<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	static void convert(const int_list& ast_val, glm::tvec2<T, p>& val) {
		val = glm::tvec2<T, p>();
		for(unsigned int i = 0; i < 2 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec3<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	static void convert(const int_list& ast_val, glm::tvec3<T, p>& val) {
		val = glm::tvec3<T, p>();
		for(unsigned int i = 0; i < 3 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec4<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	static void convert(const int_list& ast_val, glm::tvec4<T, p>& val) {
		val = glm::tvec4<T, p>();
		for(unsigned int i = 0; i < 4 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_ */
