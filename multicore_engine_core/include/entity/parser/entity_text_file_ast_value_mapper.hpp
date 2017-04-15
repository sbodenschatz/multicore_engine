/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast_value_map.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_

#include <entity/entity_reference.hpp>
#include <entity/parser/entity_text_file_ast.hpp>
#include <exceptions.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <type_traits>
#include <vector>

namespace mce {
namespace entity {
class entity_manager;
namespace ast {

/// \brief Specializations of this template implement the mapping of values from AST nodes in entity text
/// files to the C++-types (used e.g. in reflection::property).
template <typename AST_Value, typename Target, typename Dummy = void>
struct ast_value_mapper {
	/// Indicates that the base template should cause an error because the type case is not handled.
	typedef void error;
};

/// Maps integers from the AST (represented by long long) to any arithmetic type.
template <typename T>
struct ast_value_mapper<long long, T, std::enable_if_t<std::is_arithmetic<T>::value>> {
	/// Converts ast_val to T and stores it in val.
	static void convert(const long long& ast_val, T& val, entity_manager&) {
		val = T(ast_val);
	}
};

/// Maps floating point numbers from the AST (represented by float) to any floating point time.
template <typename T>
struct ast_value_mapper<float_node, T, std::enable_if_t<std::is_floating_point<T>::value>> {
	/// Converts ast_val to T and stores it in val.
	static void convert(const ast::float_node& ast_val, T& val, entity_manager&) {
		val = T(ast_val);
	}
};

/// Maps strings from the AST to string.
template <>
struct ast_value_mapper<std::string, std::string> {
	/// Converts ast_val to string and stores it in val.
	static void convert(const std::string& ast_val, std::string& val, entity_manager&) {
		val = ast_val;
	}
};

/// Maps string lists from the AST to vector<string>.
template <>
struct ast_value_mapper<string_list, std::vector<std::string>> {
	/// Converts ast_val to vector<string> and stores it in val.
	static void convert(const string_list& ast_val, std::vector<std::string>& val, entity_manager&) {
		val = ast_val;
	}
};

/// Maps float lists from the AST to float.
template <>
struct ast_value_mapper<float_list, float> {
	/// Converts ast_val to float and stores it in val.
	static void convert(const float_list& ast_val, float& val, entity_manager&) {
		val = 0.0f;
		if(ast_val.size()) {
			val = ast_val[0];
		}
	}
};

/// Maps float lists from the AST to vec2.
template <>
struct ast_value_mapper<float_list, glm::vec2> {
	/// Converts ast_val to vec2 and stores it in val.
	static void convert(const float_list& ast_val, glm::vec2& val, entity_manager&) {
		val = glm::vec2();
		for(unsigned int i = 0; i < 2 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

/// Maps float lists from the AST to vec3.
template <>
struct ast_value_mapper<float_list, glm::vec3> {
	/// Converts ast_val to vec3 and stores it in val.
	static void convert(const float_list& ast_val, glm::vec3& val, entity_manager&) {
		val = glm::vec3();
		for(unsigned int i = 0; i < 3 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

/// Maps float lists from the AST to vec4.
template <>
struct ast_value_mapper<float_list, glm::vec4> {
	/// Converts ast_val to vec4 and stores it in val.
	static void convert(const float_list& ast_val, glm::vec4& val, entity_manager&) {
		val = glm::vec4();
		for(unsigned int i = 0; i < 4 && i < ast_val.size(); ++i) {
			val[i] = ast_val[i];
		}
	}
};

/// Maps integer lists from the AST to int.
template <>
struct ast_value_mapper<int_list, int> {
	/// Converts ast_val to int and stores it in val.
	static void convert(const int_list& ast_val, int& val, entity_manager&) {
		val = 0;
		if(ast_val.size()) {
			val = ast_val[0];
		}
	}
};

/// Maps integer lists from the AST to tvec2.
template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec2<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	/// Converts ast_val to tvec2<T,p> and stores it in val.
	static void convert(const int_list& ast_val, glm::tvec2<T, p>& val, entity_manager&) {
		val = glm::tvec2<T, p>();
		for(unsigned int i = 0; i < 2 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

/// Maps integer lists from the AST to tvec3.
template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec3<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	/// Converts ast_val to tvec3<T,p> and stores it in val.
	static void convert(const int_list& ast_val, glm::tvec3<T, p>& val, entity_manager&) {
		val = glm::tvec3<T, p>();
		for(unsigned int i = 0; i < 3 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

/// Maps integer lists from the AST to tvec4.
template <typename T, glm::precision p>
struct ast_value_mapper<int_list, glm::tvec4<T, p>, std::enable_if_t<std::is_arithmetic<T>::value>> {
	/// Converts ast_val to tvec4<T,p> and stores it in val.
	static void convert(const int_list& ast_val, glm::tvec4<T, p>& val, entity_manager&) {
		val = glm::tvec4<T, p>();
		for(unsigned int i = 0; i < 4 && i < ast_val.size(); ++i) {
			val[i] = T(ast_val[i]);
		}
	}
};

/// Maps entity_references from the AST to entity_reference.
template <>
struct ast_value_mapper<ast::entity_reference, mce::entity::entity_reference> {
	/// Converts ast_val to entity_reference and stores it in val.
	static void convert(const ast::entity_reference& ast_val, mce::entity::entity_reference& val,
						entity_manager& entity_manager) {
		val = mce::entity::entity_reference(ast_val.referred_name, entity_manager);
	}
};

/// Maps float lists from the AST to tquat.
template <typename T, glm::precision p>
struct ast_value_mapper<ast::float_list, glm::tquat<T, p>> {
	/// Converts ast_val to tquat<T,p> and stores it in val.
	static void convert(const ast::float_list& ast_val, glm::tquat<T, p>& val, entity_manager&) {
		if(ast_val.empty()) val = glm::tquat<T, p>();
		if(ast_val.size() < 4) throw value_type_exception("Invalid angle-axis quaternion literal.");
		val = glm::angleAxis(glm::radians(T(ast_val[0])),
							 glm::tvec3<T, p>(ast_val[1], ast_val[2], ast_val[3]));
	}
};

/// Maps int lists from the AST to tquat.
template <typename T, glm::precision p>
struct ast_value_mapper<ast::int_list, glm::tquat<T, p>> {
	/// Converts ast_val to tquat<T,p> and stores it in val.
	static void convert(const ast::int_list& ast_val, glm::tquat<T, p>& val, entity_manager&) {
		if(ast_val.empty()) val = glm::tquat<T, p>();
		if(ast_val.size() < 4) throw value_type_exception("Invalid angle-axis quaternion literal.");
		val = glm::angleAxis(glm::radians(T(ast_val[0])),
							 glm::tvec3<T, p>(ast_val[1], ast_val[2], ast_val[3]));
	}
};

/// Maps rotation lists from the AST to tquat.
template <typename T, glm::precision p>
struct ast_value_mapper<ast::rotation_list, glm::tquat<T, p>> {
	/// Converts ast_val to tquat<T,p> and stores it in val.
	static void convert(const ast::rotation_list& ast_val, glm::tquat<T, p>& val, entity_manager&) {
		glm::tquat<T, p> orientation;
		for(auto&& entry : ast_val) {
			glm::tvec3<T, p> axis;
			switch(entry.axis) {
			case ast::rotation_axis::x: axis.x = T(1); break;
			case ast::rotation_axis::y: axis.y = T(1); break;
			case ast::rotation_axis::z: axis.z = T(1); break;
			default: throw std::logic_error("Invalid rotation axis in AST."); break;
			}
			orientation = orientation * glm::angleAxis(glm::radians(entry.angle), axis);
		}
		val = orientation;
	}
};

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_VALUE_MAPPER_HPP_ */
