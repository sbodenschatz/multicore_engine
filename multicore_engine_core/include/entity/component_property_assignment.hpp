/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_property_assignment.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_
#define ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_

#include <entity/parser/entity_text_file_ast.hpp>
#include <entity/parser/entity_text_file_ast_value_mapper.hpp>
#include <memory>
#include <reflection/property.hpp>
#include <string>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace reflection {

template <typename Root_Type, template <typename> class AbstractAssignment, typename... Assignment_Param>
class abstract_property;
template <typename Root_Type, typename T, template <typename> class AbstractAssignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
class property;

} // namespace reflection

namespace entity {
template <typename Root_Type>
class abstract_component_property_assignment {
protected:
	core::engine& engine_;
	bool valid_;

public:
	explicit abstract_component_property_assignment(core::engine& engine) noexcept : engine_(engine),
																					 valid_(false) {}
	abstract_component_property_assignment(const abstract_component_property_assignment&) = default;
	abstract_component_property_assignment(abstract_component_property_assignment&&) = default;
	abstract_component_property_assignment&
	operator=(const abstract_component_property_assignment&) = default;
	abstract_component_property_assignment& operator=(abstract_component_property_assignment&&) = default;
	virtual ~abstract_component_property_assignment() = default;
	virtual void assign(Root_Type& object) const = 0;
	virtual void parse(const ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context) = 0;
	virtual const mce::reflection::abstract_property<
			Root_Type, mce::entity::abstract_component_property_assignment, core::engine&>&
	abstract_property() noexcept = 0;
	virtual std::unique_ptr<abstract_component_property_assignment<Root_Type>> make_copy() const = 0;
	// TODO: Implement interface for binary serialization of object configurations

	bool valid() const {
		return valid_;
	}
};

template <typename Root_Type, typename T>
class component_property_assignment : public abstract_component_property_assignment<Root_Type> {
	const reflection::property<Root_Type, T, mce::entity::abstract_component_property_assignment,
							   mce::entity::component_property_assignment, core::engine&>& property_;
	T value_;

	struct ast_visitor : public boost::static_visitor<> {
		const std::string& entity_context;
		const std::string& component_context;
		component_property_assignment& pa;
		ast_visitor(const std::string& entity_context, const std::string& component_context,
					component_property_assignment& pa)
				: entity_context(entity_context), component_context(component_context), pa(pa) {}
		template <typename U, typename V = T, typename W = typename ast::ast_value_mapper<U, V>::error>
		void operator()(const U&, W* = nullptr) {
			throw value_type_exception("Invalid value for " + pa.property_.name() + " of " +
									   component_context + " in " + entity_context + ".");
		}
		template <typename U, typename V = T,
				  void (*convert)(const U&, V&) = ast::ast_value_mapper<U, V>::convert>
		void operator()(const U& ast_value) {
			convert(ast_value, pa.value_);
		}
	};

public:
	component_property_assignment(
			const mce::reflection::property<Root_Type, T, mce::entity::abstract_component_property_assignment,
											mce::entity::component_property_assignment, core::engine&>&
					property,
			core::engine& engine)
			: abstract_component_property_assignment<Root_Type>(engine), property_(property) {}
	component_property_assignment(const component_property_assignment&) = default;
	component_property_assignment(component_property_assignment&&) = default;
	component_property_assignment& operator=(const component_property_assignment&) = default;
	component_property_assignment& operator=(component_property_assignment&&) = default;
	virtual ~component_property_assignment() = default;

	virtual void assign(Root_Type& object) const override {
		if(this->valid_) property_.set_value(object, value_);
	}
	virtual void parse(const ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context) override {
		ast_visitor visitor(entity_context, component_context, *this);
		ast_value.apply_visitor(visitor);
	}
	virtual const mce::reflection::abstract_property<
			Root_Type, mce::entity::abstract_component_property_assignment, core::engine&>&
	abstract_property() noexcept override {
		return property_;
	}
	virtual std::unique_ptr<abstract_component_property_assignment<Root_Type>> make_copy() const override {
		return std::make_unique<component_property_assignment<Root_Type, T>>(*this);
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_ */
