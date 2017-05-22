/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/parser/entity_text_file_ast.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_

/**
 * \file
 * Defines AST node types for entity text files.
 */

#include <algorithm>
#include <boost/variant.hpp>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace mce {
namespace entity {
namespace ast {

/// Proxy for float to enable approximate comparison.
struct float_node {
	float value; ///< The value held by this node.
	/// Constructs an empty node (set to 0.0).
	float_node() : value(0.0f) {}
	/// Constructs a node with the given value.
	// cppcheck-suppress noExplicitConstructor
	float_node(float value) : value(value) {}
	/// Assigns a new value to the node.
	float_node& operator=(float new_value) {
		value = new_value;
		return *this;
	}
	/// Returns the value of the node as an unwrapped float.
	operator float() const {
		return value;
	}
};

/// Represents a list of integers.
typedef std::vector<long long> int_list;
/// Represents a list of floating point numbers.
typedef std::vector<float_node> float_list;
/// Represents a list of strings.
typedef std::vector<std::string> string_list;

/// Specifies the valid axis for rotations.
enum class rotation_axis {
	/// Specifies rotation around the X-axis.
	x,
	/// Specifies rotation around the Y-axis.
	y,
	/// Specifies rotation around the Z-axis.
	z
};

/// Outputs a rotation axis to a stream.
inline std::ostream& operator<<(std::ostream& s, const rotation_axis& r) {
	if(r == rotation_axis::x) {
		s << "x";
	} else if(r == rotation_axis::y) {
		s << "y";
	} else if(r == rotation_axis::z) {
		s << "z";
	} else {
		s << "<invalid rotation_axis>";
	}
	return s;
}

/// Represents an element in a list of rotations consisting of an axis and an angle.
struct rotation_element {
	rotation_axis axis; ///< The rotation axis.
	float angle;		///< The angle of the rotation in degrees.
};

/// Represents a list of rotations.
typedef std::vector<rotation_element> rotation_list;

// enum class marker_attribute { position };

/// Represents the evaluation of a marker.
struct marker_evaluation {
	// marker_attribute attribute = marker_attribute::position;

	/// The name of the marker to evaluate.
	std::string marker_name;
};

/// Represents a reference to a named entity.
struct entity_reference {
	/// The name of the referenced entity.
	std::string referred_name;
};

/// Represents any value for a variable.
typedef boost::variant<long long, float_node, std::string, int_list, float_list, string_list, rotation_list,
					   marker_evaluation, entity_reference>
		variable_value;

/// Represents a variable assignment.
struct variable {
	/// The name of the assigned variable.
	std::string name;
	/// The assigned value of the variable.
	variable_value value;
};

/// Represents a component definition.
struct component_definition {
	/// \brief Specifies if this definition replaces existing definitions for the same component, otherwise it
	/// extends them.
	bool replace = false;
	/// Specifies the name of the component.
	std::string name;
	/// The list of variable assignments for this component.
	std::vector<variable> variables;
};

/// Represents a entity definition.
struct entity_definition {
	/// Specifies the name of the entity definition.
	std::string name;
	/// Specifies the name of the entity definition from which this one inherits.
	std::string super_name;
	/// The list of component definitions for this entity definitions.
	std::vector<component_definition> components;
};

struct ast_wrapper;

/// Represents an include instruction in an entity text file.
struct include_instruction {
	/// The name of the included file.
	std::string filename;
	/// Additional annotation used in later phases:
	std::shared_ptr<ast_wrapper> included_ast;
};

/// Represents possible entity parameters.
typedef boost::variant<int_list, float_list, rotation_list, marker_evaluation, entity_reference>
		entity_instance_param;

/// Represents an instantiation of an entity.
struct entity_instance {
	/// Specifies the name of the entity type to instantiate.
	std::string type_name;
	/// Specifies the name of the instance.
	std::string instance_name;
	/// Specifies the position specification of the entity instance.
	entity_instance_param position_parameter;
	/// Specifies the orientation specification of the entity instance.
	entity_instance_param orientation_parameter;
};

/// Represents an element of the root of the entity text file AST.
typedef boost::variant<entity_definition, include_instruction, entity_instance> root_element;

/// Represents the root node of the entity text file AST.
typedef std::vector<root_element> ast_root;

/// Wraps another AST from an included entity text file.
struct ast_wrapper {
	/// The root node of the included file's AST.
	ast_root root;
	/// Constructs an ast_warpper with the given root node.
	// cppcheck-suppress passedByValue
	explicit ast_wrapper(ast_root root) : root(std::move(root)) {}
};

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_ */
