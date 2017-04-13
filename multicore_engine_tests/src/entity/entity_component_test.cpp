/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/entity/entity_component_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <asset/dummy_asset.hpp>
#include <bstream/vector_iobstream.hpp>
#include <containers/smart_object_pool.hpp>
#include <entity/component.hpp>
#include <entity/entity_manager.hpp>
#include <gtest.hpp>
#include <util/finally.hpp>

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
	test_a_system tasys;
	entity_manager em(nullptr);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, tasys.create_component_1(owner, config), &tasys);
	em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
						"Test_Ent_Conf test_ent (0,0,0),();"));
	auto test_ent = em.find_entity("test_ent");
	ASSERT_TRUE(test_ent);
	auto test_a_1_comp = test_ent->component<test_a_1_component>();
	ASSERT_TRUE(test_a_1_comp);
	ASSERT_EQ("TestComp", test_a_1_comp->name());
	ASSERT_EQ(2, test_a_1_comp->values().size());
	ASSERT_EQ("Hello", test_a_1_comp->values()[0]);
	ASSERT_EQ("World", test_a_1_comp->values()[1]);
}
TEST(entity_entity_component_test, entity_serialize_and_deserialize) {
	bstream::vector_iobstream stream;
	test_a_system tasys;
	{
		entity_manager em(nullptr);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, tasys.create_component_1(owner, config), &tasys);
		em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
							"Test_Ent_Conf test_ent (0,0,0),();"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, tasys.create_component_1(owner, config), &tasys);
		em.load_entities_from_bstream(stream);
		auto test_ent = em.find_entity("test_ent");
		ASSERT_TRUE(test_ent);
		auto test_a_1_comp = test_ent->component<test_a_1_component>();
		ASSERT_TRUE(test_a_1_comp);
		ASSERT_EQ("TestComp", test_a_1_comp->name());
		ASSERT_EQ(2, test_a_1_comp->values().size());
		ASSERT_EQ("Hello", test_a_1_comp->values()[0]);
		ASSERT_EQ("World", test_a_1_comp->values()[1]);
	}
}
TEST(entity_entity_component_test, entity_despawn) {
	test_a_system tasys;
	entity_manager em(nullptr);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, tasys.create_component_1(owner, config), &tasys);
	em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
						"Test_Ent_Conf test_ent_1 (0,0,0),();"
						"Test_Ent_Conf test_ent_2 (0,0,0),();"));
	auto test_ent_1 = em.find_entity("test_ent_1");
	auto test_ent_2 = em.find_entity("test_ent_2");
	ASSERT_TRUE(test_ent_1);
	ASSERT_TRUE(test_ent_2);
	em.destroy_entity(test_ent_1);
	test_ent_1 = em.find_entity("test_ent_1");
	test_ent_2 = em.find_entity("test_ent_2");
	ASSERT_FALSE(test_ent_1);
	ASSERT_TRUE(test_ent_2);
	em.destroy_entity(test_ent_2);
	test_ent_1 = em.find_entity("test_ent_1");
	test_ent_2 = em.find_entity("test_ent_2");
	ASSERT_FALSE(test_ent_1);
	ASSERT_FALSE(test_ent_2);
}
} // namespace entity
} // namespace mce
