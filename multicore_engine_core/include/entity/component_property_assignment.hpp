/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_property_assignment.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_
#define ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_

#include <entity/parser/entity_text_file_ast.hpp>
#include <entity/parser/entity_text_file_ast_value_mapper.hpp>
#include <exceptions.hpp>
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
class entity_manager;
/// \brief Represents the abstract base class for component_property_assignment template instances to allow
/// inserting them into a polymorphic container.
template <typename Root_Type>
class abstract_component_property_assignment {
protected:
	core::engine* engine_; ///< The referenced engine object.
	bool valid_;		   ///< Determines if the assignment has a valid value assigned.

public:
	/// Allows implementing classes to create an abstract_component_property_assignment for the given engine.
	explicit abstract_component_property_assignment(core::engine* engine) noexcept : engine_(engine),
																					 valid_(false) {}
	/// Allows copy-construction of abstract_component_property_assignment.
	abstract_component_property_assignment(const abstract_component_property_assignment&) = default;
	/// Allows move-construction of abstract_component_property_assignment.
	abstract_component_property_assignment(abstract_component_property_assignment&&) = default;
	/// Allows copy-assignment of abstract_component_property_assignment.
	abstract_component_property_assignment&
	operator=(const abstract_component_property_assignment&) = default;
	/// Allows move-assignment of abstract_component_property_assignment.
	abstract_component_property_assignment& operator=(abstract_component_property_assignment&&) = default;
	/// Enables virtual destruction for derived classes.
	virtual ~abstract_component_property_assignment() = default;
	/// Assigns the stored value to the referenced property.
	virtual void assign(Root_Type& object) const = 0;
	/// Parses a value from the given variable_value and stores it in the assignment object.
	virtual void parse(const ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context, entity_manager& entity_manager) = 0;
	/// Returns the property to which this assignment assigns a value.
	virtual const mce::reflection::abstract_property<
			Root_Type, mce::entity::abstract_component_property_assignment, core::engine*>&
	abstract_property() noexcept = 0;
	/// Returns a unique_ptr-managed copy of this assignment object.
	virtual std::unique_ptr<abstract_component_property_assignment<Root_Type>> make_copy() const = 0;
	// TODO: Implement interface for binary serialization of object configurations

	/// Returns a bool indicating if this assignment is valid.
	bool valid() const {
		return valid_;
	}
};

/// Represents a concrete assignment of a component property with a specific type of the stored value.
template <typename Root_Type, typename T>
class component_property_assignment : public abstract_component_property_assignment<Root_Type> {
	const reflection::property<Root_Type, T, mce::entity::abstract_component_property_assignment,
							   mce::entity::component_property_assignment, core::engine*>& property_;
	T value_;

	struct ast_visitor : public boost::static_visitor<> {
		const std::string& entity_context;
		const std::string& component_context;
		component_property_assignment& pa;
		entity_manager& entity_manager_;
		ast_visitor(const std::string& entity_context, const std::string& component_context,
					component_property_assignment& pa, entity_manager& entity_manager)
				: entity_context(entity_context), component_context(component_context), pa(pa),
				  entity_manager_(entity_manager) {}
		template <typename U, typename V = T, typename W = typename ast::ast_value_mapper<U, V>::error>
		void operator()(const U&, W* = nullptr) {
			throw value_type_exception("Invalid value for " + pa.property_.name() + " of " +
									   component_context + " in " + entity_context + ".");
		}
		template <typename U, typename V = T,
				  void (*convert)(const U&, V&, entity_manager&) = ast::ast_value_mapper<U, V>::convert>
		void operator()(const U& ast_value) {
			convert(ast_value, pa.value_, entity_manager_);
			pa.valid_ = true;
		}
	};

public:
	/// Constructs a component_property_assignment for the given property and referenced engine object.
	component_property_assignment(
			const mce::reflection::property<Root_Type, T, mce::entity::abstract_component_property_assignment,
											mce::entity::component_property_assignment, core::engine&>&
					property,
			core::engine& engine)
			: abstract_component_property_assignment<Root_Type>(engine), property_(property) {}
	/// Allows copy-construction of component_property_assignment.
	component_property_assignment(const component_property_assignment&) = default;
	/// Allows move-construction of component_property_assignment.
	component_property_assignment(component_property_assignment&&) = default;
	/// Allows copy-assignment of component_property_assignment.
	component_property_assignment& operator=(const component_property_assignment&) = default;
	/// Allows move-assignment of component_property_assignment.
	component_property_assignment& operator=(component_property_assignment&&) = default;
	/// Destroys the component_property_assignment object.
	virtual ~component_property_assignment() = default;
	/// Assigns the stored value to the referenced property.
	virtual void assign(Root_Type& object) const override {
		if(this->valid_) property_.set_value(object, value_);
	}
	/// Parses a value from the given variable_value and stores it in the assignment object.
	virtual void parse(const ast::variable_value& ast_value, const std::string& entity_context,
					   const std::string& component_context, entity_manager& entity_manager) override {
		ast_visitor visitor(entity_context, component_context, *this, entity_manager);
		ast_value.apply_visitor(visitor);
	}
	/// Returns the property to which this assignment assigns a value.
	virtual const mce::reflection::abstract_property<
			Root_Type, mce::entity::abstract_component_property_assignment, core::engine&>&
	abstract_property() noexcept override {
		return property_;
	}
	/// Returns a unique_ptr-managed copy of this assignment object.
	virtual std::unique_ptr<abstract_component_property_assignment<Root_Type>> make_copy() const override {
		return std::make_unique<component_property_assignment<Root_Type, T>>(*this);
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_PROPERTY_ASSIGNMENT_HPP_ */
