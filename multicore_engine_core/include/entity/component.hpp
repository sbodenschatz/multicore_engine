/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_HPP_
#define ENTITY_COMPONENT_HPP_

/**
 * \file
 * Definition of the component class.
 */

#include <core/engine.hpp>
#include <entity/component_property_assignment.hpp>
#include <reflection/property.hpp>
#include <memory>
#include <string>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {

class entity;
class component_configuration;

/// \brief Represents an abstract component of an entity (aka game object); derived classes mark the entity
/// for specific functionality and store the associated data.
class component {
private:
	entity& owner_;
	const component_configuration& configuration_;

protected:
	/// \brief Allows derived classes to construct the base class with a given owner entity and reference to a
	/// component_configuration.
	component(entity& owner, const component_configuration& configuration) noexcept
			: owner_(owner),
			  configuration_(configuration) {}

public:
	/// Specifies the type of the list of properties.
	typedef std::vector<std::unique_ptr<reflection::abstract_property<
			component, abstract_component_property_assignment, core::engine&>>> property_list;
	/// Enables virtual destruction for derived classes.
	virtual ~component() = default;
	/// Allows access to the component_configuration from which this component object was created.
	const component_configuration& configuration() const {
		return configuration_;
	}
	/// Allows access to the entity owning this component.
	const entity& owner() const {
		return owner_;
	}
	/// Allows access to the entity owning this component.
	entity& owner() {
		return owner_;
	}
	/// \brief The fill_property_list static member function of derived classes is called by component_type to
	/// register the properties of the derived class. If the derived class specifies no such method this no-op
	/// implementation is used through inheritance.
	static void fill_property_list(property_list& properties);

	/// Stores the current state of the components (it's property values) to the given bstream.
	void store_to_bstream(bstream::obstream& ostr) const;
	/// Loads the state of the component (as stored by store_to_bstream) from the given bstream.
	void load_from_bstream(bstream::ibstream& istr);

protected:
	/// \brief Allows derived classes to register a property specified by the given name, getter and setter to
	/// the given list of properties.
	template <typename T, typename Comp>
	static void register_component_property(
			property_list& list, const std::string& name,
			typename reflection::linked_property<component, T, Comp, abstract_component_property_assignment,
												 component_property_assignment, core::engine&>::getter_t
					getter,
			typename reflection::linked_property<component, T, Comp, abstract_component_property_assignment,
												 component_property_assignment, core::engine&>::setter_t
					setter) {
		list.emplace_back(
				reflection::make_property<component, T, Comp, abstract_component_property_assignment,
										  component_property_assignment, core::engine&>(name, getter,
																						setter));
	}
	/// \brief Allows derived classes to register a property specified by the given name and member variable
	/// pointer to the given list of properties.
	template <typename T, typename Comp>
	static void
	register_component_property(property_list& list, const std::string& name,
								typename reflection::directly_linked_property<
										component, T, Comp, abstract_component_property_assignment,
										component_property_assignment, core::engine&>::variable_t variable) {
		list.emplace_back(
				reflection::make_property<component, abstract_component_property_assignment,
										  component_property_assignment, core::engine&>(name, variable));
	}
};

} // namespace entity
} // namespace mce

/// \brief Allows more comfortable registration of properties by applying the convention that the property
/// uses a getter and a setter that are both named like the property and differentiate by signature.
#define REGISTER_COMPONENT_PROPERTY(LIST, COMP, TYPE, NAME)                                                  \
	register_component_property<TYPE, COMP>(                                                                 \
			LIST, #NAME,                                                                                     \
			static_cast<mce::reflection::linked_property<                                                    \
					component, TYPE, COMP, mce::entity::abstract_component_property_assignment,              \
					mce::entity::component_property_assignment, mce::core::engine&>::getter_t>(&COMP::NAME), \
			static_cast<mce::reflection::linked_property<                                                    \
					component, TYPE, COMP, mce::entity::abstract_component_property_assignment,              \
					mce::entity::component_property_assignment, mce::core::engine&>::setter_t>(&COMP::NAME))

/// \brief Allows more comfortable registration of properties by applying the convention that the property is
/// represented by a member variable that has the same name as the property.
#define REGISTER_COMPONENT_PROPERTY_DIRECT(LIST, COMP, TYPE, NAME)                                           \
	register_component_property<TYPE, COMP>(LIST, #NAME, &COMP::NAME)

#endif /* ENTITY_COMPONENT_HPP_ */
