/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/parser/entity_template_lang_variable_conversion.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_ENTITY_PARSER_ENTITY_TEMPLATE_LANG_VARIABLE_CONVERSION_HPP_
#define MCE_ENTITY_PARSER_ENTITY_TEMPLATE_LANG_VARIABLE_CONVERSION_HPP_

/**
 * \file
 * Provides helper conversion functions to work with entity template language AST variables.
 */

#include <mce/entity/parser/entity_template_lang_ast.hpp>
#include <mce/entity/parser/entity_template_lang_ast_value_mapper.hpp>

namespace mce {
namespace entity {
class entity_manager;
namespace ast {

namespace detail {

template <typename T>
struct get_variable_value_ast_visitor : public boost::static_visitor<T> {
	mce::entity::entity_manager& entity_manager_;
	explicit get_variable_value_ast_visitor(entity_manager& entity_manager)
			: entity_manager_(entity_manager) {}
	template <typename U, typename V = T, typename W = typename ast::ast_value_mapper<U, V>::error>
	T operator()(const U&, W* = nullptr) {
		throw value_type_exception("Invalid value for requested type.");
	}
	template <typename U, typename V = T,
			  void (*convert)(const U&, V&, entity_manager&) = ast::ast_value_mapper<U, V>::convert>
	T operator()(const U& ast_value) {
		try {
			T val;
			ast::ast_value_mapper<U, V>::convert(ast_value, val, entity_manager_);
			return val;
		} catch(value_type_exception& ex) {
			using namespace std::literals;
			throw value_type_exception("Error '"s + ex.what() + "' getting variable value.");
		}
	}
};

} // namespace detail

/// \brief Helper function to obtain a value of type <code>T</code> from a given AST variable_value by
/// performing the conversions specified by ast_value_mapper specializations.
/**
 * The entity_manager is required to resolve entity_reference values.
 */
template <typename T>
T get_variable_value(const variable_value& var, entity_manager& em) {
	detail::get_variable_value_ast_visitor<T> v(em);
	return var.apply_visitor(v);
}

} // namespace ast
} // namespace entity
} // namespace mce

#endif /* MCE_ENTITY_PARSER_ENTITY_TEMPLATE_LANG_VARIABLE_CONVERSION_HPP_ */
