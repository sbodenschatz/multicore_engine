/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/entity/entity_component_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <containers/smart_object_pool.hpp>
#include <entity/component.hpp>
#include <entity/entity_manager.hpp>
#include <gtest.hpp>

namespace mce {
namespace entity {

class test_a_1_component : public component {
private:
	std::string name_;
	std::vector<std::string> values_;

public:
	test_a_1_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	const std::string& name() const {
		return name_;
	}

	void name(const std::string& name) {
		name_ = name;
	}

	const std::vector<std::string>& values() const {
		return values_;
	}

	void values(const std::vector<std::string>& values) {
		values_ = values;
	}
	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_a_1_component, std::string, name);
		REGISTER_COMPONENT_PROPERTY(prop, test_a_1_component, std::vector<std::string>, values);
	}
};

class test_a_system {
	containers::smart_object_pool<test_a_1_component, 256> components1;

public:
	containers::smart_pool_ptr<test_a_1_component>
	create_component_1(entity& owner, const component_configuration& configuration) {
		return components1.emplace(owner, configuration);
	}
};

TEST(entity_entity_component_test, load_entity_with_simple_component) {
	entity_manager em(nullptr);
	test_a_system tasys;
	REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, tasys.create_component_1(owner, config), &tasys);
}

} // namespace entity
} // namespace mce
