/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_

#include <boost/variant.hpp>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace mce {
namespace entity {
namespace ast {

// Proxy for float to enable approximate comparison.
struct float_node {
	float value;
	float_node() : value(0.0f) {}
	// cppcheck-suppress noExplicitConstructor
	float_node(float value) : value(value) {}
	float_node& operator=(float new_value) {
		value = new_value;
		return *this;
	}
	operator float() const {
		return value;
	}
};

typedef std::vector<long long> int_list;
typedef std::vector<float_node> float_list;
typedef std::vector<std::string> string_list;

enum class rotation_axis { x, y, z };
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

struct rotation_element {
	rotation_axis axis;
	float angle;
};

typedef std::vector<rotation_element> rotation_list;

// enum class marker_attribute { position };
struct marker_evaluation {
	// marker_attribute attribute = marker_attribute::position;
	std::string marker_name;
};

struct entity_reference {
	std::string referred_name;
};

typedef boost::variant<long long, float_node, std::string, int_list, float_list, string_list, rotation_list,
					   marker_evaluation, entity_reference>
		variable_value;

struct variable {
	std::string name;
	variable_value value;
};

struct component_definition {
	bool replace = false;
	std::string name;
	std::vector<variable> variables;
};

struct entity_definition {
	std::string name;
	std::string super_name;
	std::vector<component_definition> components;
};

struct ast_wrapper;

struct include_instruction {
	std::string filename;
	// Additional annotation used in later phases:
	std::shared_ptr<ast_wrapper> included_ast;
};

typedef boost::variant<int_list, float_list, rotation_list, marker_evaluation, entity_reference>
		entity_instance_param;

struct entity_instance {
	std::string type_name;
	std::string instance_name;
	entity_instance_param position_parameter;
	entity_instance_param orientation_parameter;
};

typedef boost::variant<entity_definition, include_instruction, entity_instance> root_element;

typedef std::vector<root_element> ast_root;

struct ast_wrapper {
	ast_root root;
	// cppcheck-suppress passedByValue
	explicit ast_wrapper(ast_root root) : root(std::move(root)) {}
};

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_ */
