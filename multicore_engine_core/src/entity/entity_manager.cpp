/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_manager.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/entity_manager.hpp>
#include <entity/entity_configuration.hpp>
#include <entity/component_type.hpp>

#include <entity/component.hpp>
#include <containers/smart_pool_ptr.hpp>

class test_component : public mce::entity::component {
public:
	int x() const {
		return x_;
	}

	void x(int x) {
		this->x_ = x;
	}

	static void fill_property_list(property_list& properties);

private:
	int x_;
	int y;
};

void test_component::fill_property_list(property_list& properties) {
	REGISTER_COMPONENT_PROPERTY(properties, test_component, int, x);
	REGISTER_COMPONENT_PROPERTY_DIRECT(properties, test_component, int, y);
}

#define REGISTER_COMPONENT_TYPE(TYPE, NAME, FACTORYEXPR)                                                     \
	register_component_type<TYPE>(NAME,                                                                      \
								  [](auto&& owner, auto&& config, auto&& engine) { return FACTORYEXPR; })

#define REGISTER_COMPONENT_TYPE_SIMPLE(NAME, FACTORYEXPR)                                                    \
	register_component_type<NAME##_component>(                                                               \
			#NAME, [](auto&& owner, auto&& config, auto&& engine) { return FACTORYEXPR; })

namespace mce {
namespace entity {

entity_manager::entity_manager(core::engine& engine) : engine(engine) {
	REGISTER_COMPONENT_TYPE_SIMPLE(test, containers::smart_pool_ptr<test_component>());
}

entity_manager::~entity_manager() {}

} // namespace entity
} // namespace mce
