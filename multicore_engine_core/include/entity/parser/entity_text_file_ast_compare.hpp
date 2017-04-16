/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast_compare.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_

/**
 * \file
 * Enables comparison for AST node types for entity text files.
 */

#include <boost/variant/variant.hpp>
#include <cmath>
#include <entity/parser/entity_text_file_ast.hpp>
#include <limits>
#include <string>
#include <vector>

namespace mce {
namespace entity {
namespace ast {

/// Enables comparison of float_nodes for equal.
inline bool operator==(const float_node& o1, const float_node& o2) {
	return fabsf(o1.value - o2.value) < std::numeric_limits<float>::epsilon();
}
/// Enables comparison of float_nodes for not equal.
inline bool operator!=(const float_node& o1, const float_node& o2) {
	return !(o1 == o2);
}

/// Enables comparison of rotation_elements for equal.
inline bool operator==(const rotation_element& o1, const rotation_element& o2) {
	return o1.axis == o2.axis && fabsf(o1.angle - o2.angle) < std::numeric_limits<float>::epsilon();
}
/// Enables comparison of rotation_elements for not equal.
inline bool operator!=(const rotation_element& o1, const rotation_element& o2) {
	return !(o1 == o2);
}

/// Enables comparison of marker_evaluations for equal.
inline bool operator==(const marker_evaluation& o1, const marker_evaluation& o2) {
	return o1.marker_name == o2.marker_name;
}
/// Enables comparison of marker_evaluations for not equal.
inline bool operator!=(const marker_evaluation& o1, const marker_evaluation& o2) {
	return !(o1 == o2);
}

/// Enables comparison of entity_references for equal.
inline bool operator==(const entity_reference& o1, const entity_reference& o2) {
	return o1.referred_name == o2.referred_name;
}
/// Enables comparison of entity_references for not equal.
inline bool operator!=(const entity_reference& o1, const entity_reference& o2) {
	return !(o1 == o2);
}

/// Enables comparison of variables for equal.
inline bool operator==(const variable& o1, const variable& o2) {
	return o1.name == o2.name && o1.value == o2.value;
}
/// Enables comparison of variables for not equal.
inline bool operator!=(const variable& o1, const variable& o2) {
	return !(o1 == o2);
}

/// Enables comparison of component_definitions for equal.
inline bool operator==(const component_definition& o1, const component_definition& o2) {
	return o1.replace == o2.replace && o1.name == o2.name && o1.variables == o2.variables;
}
/// Enables comparison of component_definitions for not equal.
inline bool operator!=(const component_definition& o1, const component_definition& o2) {
	return !(o1 == o2);
}

/// Enables comparison of entity_definitions for equal.
inline bool operator==(const entity_definition& o1, const entity_definition& o2) {
	return o1.name == o2.name && o1.super_name == o2.super_name && o1.components == o2.components;
}
/// Enables comparison of entity_definitions for not equal.
inline bool operator!=(const entity_definition& o1, const entity_definition& o2) {
	return !(o1 == o2);
}

/// Enables comparison of include_instructions for equal.
inline bool operator==(const include_instruction& o1, const include_instruction& o2) {
	return o1.filename == o2.filename;
}
/// Enables comparison of include_instructions for not equal.
inline bool operator!=(const include_instruction& o1, const include_instruction& o2) {
	return !(o1 == o2);
}

/// Enables comparison of entity_instances for equal.
inline bool operator==(const entity_instance& o1, const entity_instance& o2) {
	return o1.type_name == o2.type_name && o1.instance_name == o2.instance_name &&
		   o1.position_parameter == o2.position_parameter &&
		   o1.orientation_parameter == o2.orientation_parameter;
}
/// Enables comparison of entity_instances for not equal.
inline bool operator!=(const entity_instance& o1, const entity_instance& o2) {
	return !(o1 == o2);
}

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_COMPARE_HPP_ */
