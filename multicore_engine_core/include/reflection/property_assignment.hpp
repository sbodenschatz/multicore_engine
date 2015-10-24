/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/property_assignment.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_ASSIGNMENT_HPP_
#define REFLECTION_PROPERTY_ASSIGNMENT_HPP_

#include <memory>
#include <string>
#include "property.hpp"
#include <entity/parser/entity_text_file_ast.hpp>
#include <entity/parser/entity_text_file_ast_value_mapper.hpp>

namespace mce {
namespace reflection {

template <typename Root_Type>
class abstract_property;
template <typename Root_Type, typename T>
class property;

template <typename Root_Type>
class abstract_property_assignment {
protected:
	bool valid_;

public:
	abstract_property_assignment() noexcept : valid_(false) {}
	abstract_property_assignment(const abstract_property_assignment&) = default;
	abstract_property_assignment(abstract_property_assignment&&) = default;
	abstract_property_assignment& operator=(const abstract_property_assignment&) = default;
	abstract_property_assignment& operator=(abstract_property_assignment&&) = default;
	virtual ~abstract_property_assignment() = default;
	virtual void assign(Root_Type& object) const = 0;
	virtual void parse(const entity::ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context) = 0;
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept = 0;
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_copy() const = 0;
	// TODO: Implement interface for binary serialization of object configurations

	bool valid() const {
		return valid_;
	}
};

template <typename Root_Type, typename T>
class property_assignment : public abstract_property_assignment<Root_Type> {
	const property<Root_Type, T>& property_;
	T value_;

	struct ast_visitor : public boost::static_visitor<> {
		const std::string& entity_context;
		const std::string& component_context;
		property_assignment& pa;
		ast_visitor(const std::string& entity_context, const std::string& component_context,
					property_assignment& pa)
				: entity_context(entity_context), component_context(component_context), pa(pa) {}
		template <typename U, typename V = T,
				  typename W = typename entity::ast::ast_value_mapper<U, V>::error>
		void operator()(const U&, W* dummy = nullptr) {
			throw std::runtime_error("Invalid value for " + pa.property_.name() + " of " + component_context +
									 " in " + entity_context + ".");
		}
		template <typename U, typename V = T,
				  void (*convert)(const U&, V&) = entity::ast::ast_value_mapper<U, V>::convert>
		void operator()(const U& ast_value) {
			convert(ast_value, pa.value_);
		}
	};

public:
	property_assignment(const mce::reflection::property<Root_Type, T>& property) : property_(property) {}
	property_assignment(const property_assignment&) = default;
	property_assignment(property_assignment&&) = default;
	property_assignment& operator=(const property_assignment&) = default;
	property_assignment& operator=(property_assignment&&) = default;
	virtual ~property_assignment() = default;

	virtual void assign(Root_Type& object) const override {
		if(this->valid_) property_.set_value(object, value_);
	}
	virtual void parse(const entity::ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context) override {
		ast_visitor visitor(entity_context, component_context, *this);
		ast_value.apply_visitor(visitor);
	}
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept override {
		return property_;
	}
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_copy() const override {
		return std::make_unique<property_assignment<Root_Type, T>>(*this);
	}
};

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_ASSIGNMENT_HPP_ */
