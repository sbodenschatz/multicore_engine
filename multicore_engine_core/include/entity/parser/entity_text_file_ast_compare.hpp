/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast_compare.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_

#include "entity_text_file_ast.hpp"
#include <limits>

namespace mce {
namespace entity {
namespace ast {

inline bool operator==(const float_node& o1, const float_node& o2) {
	return fabsf(o1.value - o2.value) < std::numeric_limits<float>::epsilon();
}
inline bool operator!=(const float_node& o1, const float_node& o2) {
	return !(o1 == o2);
}

inline bool operator==(const rotation_element& o1, const rotation_element& o2) {
	return o1.axis == o2.axis && fabsf(o1.angle - o2.angle) < std::numeric_limits<float>::epsilon();
}
inline bool operator!=(const rotation_element& o1, const rotation_element& o2) {
	return !(o1 == o2);
}

inline bool operator==(const marker_evaluation& o1, const marker_evaluation& o2) {
	return o1.marker_name == o2.marker_name;
}
inline bool operator!=(const marker_evaluation& o1, const marker_evaluation& o2) {
	return !(o1 == o2);
}

inline bool operator==(const entity_reference& o1, const entity_reference& o2) {
	return o1.referred_name == o2.referred_name;
}
inline bool operator!=(const entity_reference& o1, const entity_reference& o2) {
	return !(o1 == o2);
}

inline bool operator==(const variable& o1, const variable& o2) {
	return o1.name == o2.name && o1.value == o2.value;
}
inline bool operator!=(const variable& o1, const variable& o2) {
	return !(o1 == o2);
}

inline bool operator==(const component_definition& o1, const component_definition& o2) {
	return o1.replace == o2.replace && o1.name == o2.name && o1.variables == o2.variables;
}
inline bool operator!=(const component_definition& o1, const component_definition& o2) {
	return !(o1 == o2);
}

inline bool operator==(const entity_definition& o1, const entity_definition& o2) {
	return o1.name == o2.name && o1.super_name == o2.super_name && o1.components == o2.components;
}
inline bool operator!=(const entity_definition& o1, const entity_definition& o2) {
	return !(o1 == o2);
}

inline bool operator==(const include_instruction& o1, const include_instruction& o2) {
	return o1.filename == o2.filename;
}
inline bool operator!=(const include_instruction& o1, const include_instruction& o2) {
	return !(o1 == o2);
}

inline bool operator==(const entity_instance& o1, const entity_instance& o2) {
	return o1.type_name == o2.type_name && o1.instance_name == o2.instance_name &&
		   o1.position_parameter == o2.position_parameter &&
		   o1.orientation_parameter == o2.orientation_parameter;
}
inline bool operator!=(const entity_instance& o1, const entity_instance& o2) {
	return !(o1 == o2);
}

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_ */
