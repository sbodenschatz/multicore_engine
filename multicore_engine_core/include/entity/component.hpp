/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_HPP_
#define ENTITY_COMPONENT_HPP_

#include <vector>
#include <memory>
#include <reflection/property.hpp>

namespace mce {
namespace entity {

class entity;
class component_configuration;

class component {
private:
	entity& owner_;
	const component_configuration& configuration_;

protected:
	component(entity& owner, component_configuration& configuration) noexcept
			: owner_(owner),
			  configuration_(configuration) {}

public:
	typedef std::vector<std::unique_ptr<reflection::abstract_property<component>>> property_list;
	virtual ~component() = default;

	const component_configuration& configuration() const {
		return configuration_;
	}

	const entity& owner() const {
		return owner_;
	}

	entity& owner() {
		return owner_;
	}

	static void fill_property_list(property_list& properties);

protected:
	template <typename T, typename Comp>
	static void
	register_component_property(property_list& list, const std::string& name,
								typename reflection::linked_property<component, T, Comp>::getter_t getter,
								typename reflection::linked_property<component, T, Comp>::setter_t setter) {
		list.emplace_back(reflection::make_property<component, T, Comp>(name, getter, setter));
	}
	template <typename T, typename Comp>
	static void register_component_property(
			property_list& list, const std::string& name,
			typename reflection::directly_linked_property<component, T, Comp>::variable_t variable) {
		list.emplace_back(reflection::make_property<component>(name, variable));
	}
};

} // namespace entity
} // namespace mce

#define REGISTER_COMPONENT_PROPERTY(LIST, COMP, TYPE, NAME)                                                  \
	register_component_property<TYPE, COMP>(                                                                 \
			LIST, #NAME,                                                                                     \
			static_cast<mce::reflection::linked_property<component, TYPE, COMP>::getter_t>(&COMP::NAME),     \
			static_cast<mce::reflection::linked_property<component, TYPE, COMP>::setter_t>(&COMP::NAME))

#define REGISTER_COMPONENT_PROPERTY_DIRECT(LIST, COMP, TYPE, NAME)                                           \
	register_component_property<TYPE, COMP>(LIST, #NAME, &COMP::NAME)

#endif /* ENTITY_COMPONENT_HPP_ */
