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

	void register_with_manager(entity_manager& em) {
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_a_1, this->create_component_1(owner, config), this);
	}
};

static void simple_ecs_test_verfiy(entity_manager& em) {
	auto test_ent = em.find_entity("test_ent");
	ASSERT_TRUE(test_ent);
	auto test_a_1_comp = test_ent->component<test_a_1_component>();
	ASSERT_TRUE(test_a_1_comp);
	ASSERT_EQ("TestComp", test_a_1_comp->name());
	ASSERT_EQ(2, test_a_1_comp->values().size());
	ASSERT_EQ("Hello", test_a_1_comp->values()[0]);
	ASSERT_EQ("World", test_a_1_comp->values()[1]);
}
TEST(entity_entity_component_test, load_entity_with_simple_component) {
	test_a_system tasys;
	entity_manager em(nullptr);
	tasys.register_with_manager(em);
	em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
						"Test_Ent_Conf test_ent (0,0,0),();"));
	simple_ecs_test_verfiy(em);
}
TEST(entity_entity_component_test, entity_serialize_and_deserialize) {
	bstream::vector_iobstream stream;
	test_a_system tasys;
	{
		entity_manager em(nullptr);
		tasys.register_with_manager(em);
		em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
							"Test_Ent_Conf test_ent (0,0,0),();"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		tasys.register_with_manager(em);
		em.load_entities_from_bstream(stream);
		simple_ecs_test_verfiy(em);
	}
}
TEST(entity_entity_component_test, entity_despawn) {
	test_a_system tasys;
	entity_manager em(nullptr);
	tasys.register_with_manager(em);
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

class test_b_entref_component : public component {
private:
	entity_reference ent_ref_;

public:
	test_b_entref_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_b_entref_component, entity_reference, ent_ref);
	}

	const entity_reference& ent_ref() const {
		return ent_ref_;
	}

	void ent_ref(const entity_reference& ent_ref) {
		ent_ref_ = ent_ref;
	}
};

class test_b_quat_component : public component {
private:
	glm::quat orientation_;

public:
	test_b_quat_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_b_quat_component, glm::quat, orientation);
	}

	const glm::quat& orientation() const {
		return orientation_;
	}

	void orientation(const glm::quat& orientation) {
		orientation_ = orientation;
	}
};

class test_b_system {
	containers::smart_object_pool<test_b_entref_component, 256> entref_components;
	containers::smart_object_pool<test_b_quat_component, 256> quat_components;

public:
	containers::smart_pool_ptr<test_b_entref_component>
	create_entref_component(entity& owner, const component_configuration& configuration) {
		return entref_components.emplace(owner, configuration);
	}
	containers::smart_pool_ptr<test_b_quat_component>
	create_quat_component(entity& owner, const component_configuration& configuration) {
		return quat_components.emplace(owner, configuration);
	}

	void register_with_manager(entity_manager& em) {
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_entref, this->create_entref_component(owner, config), this);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_quat, this->create_quat_component(owner, config), this);
	}
};

static void entity_component_property_entity_reference_verfiy(entity_manager& em) {
	auto test_ent1 = em.find_entity("test_ent1");
	auto test_ent2 = em.find_entity("test_ent2");
	ASSERT_TRUE(test_ent2);
	auto test_ent2_ent_ref_comp = test_ent2->component<test_b_entref_component>();
	ASSERT_TRUE(test_ent2_ent_ref_comp);
	auto ent_ref = test_ent2_ent_ref_comp->ent_ref().resolve();
	ASSERT_TRUE(ent_ref);
	ASSERT_EQ(test_ent1, ent_ref);
}

TEST(entity_entity_component_test, entity_component_property_entity_reference) {
	test_a_system tasys;
	test_b_system tbsys;
	entity_manager em(nullptr);
	tasys.register_with_manager(em);
	tbsys.register_with_manager(em);
	em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
						"Test2_Ent_Conf{test_b_entref{ent_ref=entity test_ent1;}}"
						"Test_Ent_Conf test_ent1 (0,0,0),();"
						"Test2_Ent_Conf test_ent2 (0,0,0),();"));
	entity_component_property_entity_reference_verfiy(em);
}

TEST(entity_entity_component_test, entity_component_property_entity_reference_serialize_deserialize) {
	bstream::vector_iobstream stream;
	test_a_system tasys;
	test_b_system tbsys;
	{
		entity_manager em(nullptr);
		tasys.register_with_manager(em);
		tbsys.register_with_manager(em);
		em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf{test_a_1{name=\"TestComp\";values=(\"Hello\",\"World\");}}"
							"Test2_Ent_Conf{test_b_entref{ent_ref=entity test_ent1;}}"
							"Test_Ent_Conf test_ent1 (0,0,0),();"
							"Test2_Ent_Conf test_ent2 (0,0,0),();"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		tasys.register_with_manager(em);
		tbsys.register_with_manager(em);
		em.load_entities_from_bstream(stream);
		entity_component_property_entity_reference_verfiy(em);
	}
}

static void entity_component_property_quaternion_verify(entity_manager& em) {
	auto test_ent1 = em.find_entity("test_ent1");
	auto test_ent2 = em.find_entity("test_ent2");
	ASSERT_TRUE(test_ent1);
	ASSERT_TRUE(test_ent2);
	auto test_ent1_quat_comp = test_ent1->component<test_b_quat_component>();
	auto test_ent2_quat_comp = test_ent2->component<test_b_quat_component>();
	ASSERT_TRUE(test_ent1_quat_comp);
	ASSERT_TRUE(test_ent2_quat_comp);
	glm::quat expected = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ASSERT_FLOAT_EQ(expected.x, test_ent1_quat_comp->orientation().x);
	ASSERT_FLOAT_EQ(expected.y, test_ent1_quat_comp->orientation().y);
	ASSERT_FLOAT_EQ(expected.z, test_ent1_quat_comp->orientation().z);
	ASSERT_FLOAT_EQ(expected.w, test_ent1_quat_comp->orientation().w);

	ASSERT_FLOAT_EQ(expected.x, test_ent2_quat_comp->orientation().x);
	ASSERT_FLOAT_EQ(expected.y, test_ent2_quat_comp->orientation().y);
	ASSERT_FLOAT_EQ(expected.z, test_ent2_quat_comp->orientation().z);
	ASSERT_FLOAT_EQ(expected.w, test_ent2_quat_comp->orientation().w);
}

TEST(entity_entity_component_test, entity_component_property_quaternion) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf_1{test_b_quat{orientation=(x:90,y:0,z:0);}}"
						"Test_Ent_Conf_2{test_b_quat{orientation=(90,1,0,0);}}"
						"Test_Ent_Conf_1 test_ent1 (0,0,0),(x:90,y:0,z:0);"
						"Test_Ent_Conf_2 test_ent2 (0,0,0),(90,1,0,0);"));
	entity_component_property_quaternion_verify(em);
}

TEST(entity_entity_component_test, entity_component_property_quaternion_serialize_deserialize) {
	bstream::vector_iobstream stream;
	test_b_system tbsys;
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_text_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf_1{test_b_quat{orientation=(x:90,y:0,z:0);}}"
							"Test_Ent_Conf_2{test_b_quat{orientation=(90,1,0,0);}}"
							"Test_Ent_Conf_1 test_ent1 (0,0,0),(x:90,y:0,z:0);"
							"Test_Ent_Conf_2 test_ent2 (0,0,0),(90,1,0,0);"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_bstream(stream);
		entity_component_property_quaternion_verify(em);
	}
}

} // namespace entity
} // namespace mce
