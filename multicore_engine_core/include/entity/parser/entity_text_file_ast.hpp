/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_ast.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include <boost/variant.hpp>

namespace mce {
namespace entity {
namespace ast {

typedef std::vector<int64_t> int_list;
typedef std::vector<float> float_list;
typedef std::vector<std::string> string_list;

enum class rotation_axis { x, y, z };
struct rotation_element {
	rotation_axis axis;
	float angle;
};
typedef std::vector<rotation_element> rotation_list;

enum class marker_attribute { position };
struct marker_evaluation {
	marker_attribute attribute;
	std::string marker_name;
};

enum class entity_attribute { id, position, orientation };
struct entity_reference {
	entity_attribute attribute = entity_attribute::id;
	std::string referred_name;
};

typedef boost::variant<int64_t, float, std::string, int_list, float_list, string_list, rotation_list,
					   marker_evaluation, entity_reference> variable_value;

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

struct include_instruction {
	std::string filename;
};

typedef boost::variant<int_list, float_list, rotation_list, marker_evaluation, entity_reference>
		entity_instance_param;

struct entity_instance {
	std::string type_name;
	std::string instance_name;
	entity_instance_param position_parameter;
	entity_instance_param orientation_parameter;
};

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_HPP_ */
