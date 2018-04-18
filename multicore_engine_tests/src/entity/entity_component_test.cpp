/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/entity/entity_component_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/asset/dummy_asset.hpp>
#include <mce/bstream/vector_iobstream.hpp>
#include <mce/containers/simple_smart_object_pool.hpp>
#include <mce/containers/smart_object_pool.hpp>
#include <mce/entity/component.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/util/finally.hpp>

namespace mce {
namespace entity {

class test_a_1_component : public component {
private:
	std::string name_;
	std::vector<std::string> values_;

public:
	test_a_1_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	std::string name() const {
		return name_;
	}

	void name(const std::string& name) {
		name_ = name;
	}

	std::vector<std::string> values() const {
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
	component_pool<test_a_1_component, 256> components1;

public:
	component_impl_pool_ptr<test_a_1_component>
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
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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
		em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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

	entity_reference ent_ref() const {
		return ent_ref_;
	}

	void ent_ref(const entity_reference& ent_ref) {
		ent_ref_ = ent_ref;
	}
};

class test_b_quat_component : public component {
private:
	glm::quat orientation_{1.0f,0.0f,0.0f,0.0f};

public:
	test_b_quat_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_b_quat_component, glm::quat, orientation);
	}

	glm::quat orientation() const {
		return orientation_;
	}

	void orientation(const glm::quat& orientation) {
		orientation_ = orientation;
	}
};

class test_b_float_component : public component {
private:
	float scalar_ = 0.0f;
	glm::vec2 vec2_{ 0.0f };
	glm::vec3 vec3_{ 0.0f };
	glm::vec4 vec4_{ 0.0f };

public:
	test_b_float_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_b_float_component, float, scalar);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_float_component, glm::vec2, vec2);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_float_component, glm::vec3, vec3);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_float_component, glm::vec4, vec4);
	}

	float scalar() const {
		return scalar_;
	}

	void scalar(float scalar) {
		scalar_ = scalar;
	}

	glm::vec2 vec2() const {
		return vec2_;
	}

	void vec2(const glm::vec2& vec2) {
		vec2_ = vec2;
	}

	glm::vec3 vec3() const {
		return vec3_;
	}

	void vec3(const glm::vec3& vec3) {
		vec3_ = vec3;
	}

	glm::vec4 vec4() const {
		return vec4_;
	}

	void vec4(const glm::vec4& vec4) {
		vec4_ = vec4;
	}
};

class test_b_int_component : public component {
private:
	int scalar_ = 0;
	glm::ivec2 vec2_{ 0 };
	glm::ivec3 vec3_{ 0 };
	glm::ivec4 vec4_{ 0 };

public:
	test_b_int_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY(prop, test_b_int_component, int, scalar);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_int_component, glm::ivec2, vec2);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_int_component, glm::ivec3, vec3);
		REGISTER_COMPONENT_PROPERTY(prop, test_b_int_component, glm::ivec4, vec4);
	}

	int scalar() const {
		return scalar_;
	}

	void scalar(int scalar) {
		scalar_ = scalar;
	}

	glm::ivec2 vec2() const {
		return vec2_;
	}

	void vec2(const glm::ivec2& vec2) {
		vec2_ = vec2;
	}

	glm::ivec3 vec3() const {
		return vec3_;
	}

	void vec3(const glm::ivec3& vec3) {
		vec3_ = vec3;
	}

	glm::ivec4 vec4() const {
		return vec4_;
	}

	void vec4(const glm::ivec4& vec4) {
		vec4_ = vec4;
	}
};

class test_b_direct_prop_component : public component {
private:
	int int_val = 0;
	std::string string_val;

public:
	test_b_direct_prop_component(entity& owner, const component_configuration& configuration) noexcept
			: component(owner, configuration) {}

	static void fill_property_list(property_list& prop) {
		REGISTER_COMPONENT_PROPERTY_DIRECT(prop, test_b_direct_prop_component, int, int_val);
		REGISTER_COMPONENT_PROPERTY_DIRECT(prop, test_b_direct_prop_component, std::string, string_val);
	}

	int get_int_val() const {
		return int_val;
	}

	const std::string& get_string_val() const {
		return string_val;
	}
};

class test_b_system {
	component_pool<test_b_entref_component, 256> entref_components;
	component_pool<test_b_quat_component, 256> quat_components;
	component_pool<test_b_float_component, 256> float_components;
	component_pool<test_b_int_component, 256> int_components;
	component_pool<test_b_direct_prop_component, 256> direct_prop_components;

public:
	component_impl_pool_ptr<test_b_entref_component>
	create_entref_component(entity& owner, const component_configuration& configuration) {
		return entref_components.emplace(owner, configuration);
	}
	component_impl_pool_ptr<test_b_quat_component>
	create_quat_component(entity& owner, const component_configuration& configuration) {
		return quat_components.emplace(owner, configuration);
	}
	component_impl_pool_ptr<test_b_float_component>
	create_float_component(entity& owner, const component_configuration& configuration) {
		return float_components.emplace(owner, configuration);
	}
	component_impl_pool_ptr<test_b_int_component>
	create_int_component(entity& owner, const component_configuration& configuration) {
		return int_components.emplace(owner, configuration);
	}
	component_impl_pool_ptr<test_b_direct_prop_component>
	create_direct_prop_component(entity& owner, const component_configuration& configuration) {
		return direct_prop_components.emplace(owner, configuration);
	}

	void register_with_manager(entity_manager& em) {
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_entref, this->create_entref_component(owner, config), this);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_quat, this->create_quat_component(owner, config), this);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_float, this->create_float_component(owner, config), this);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_int, this->create_int_component(owner, config), this);
		REGISTER_COMPONENT_TYPE_SIMPLE(em, test_b_direct_prop,
									   this->create_direct_prop_component(owner, config), this);
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
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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
		em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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
		em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
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

static void entity_component_property_float_verify(entity_manager& em) {
	auto test_ent = em.find_entity("test_ent1");
	ASSERT_TRUE(test_ent);
	auto test_ent_float_comp = test_ent->component<test_b_float_component>();
	ASSERT_TRUE(test_ent_float_comp);
	float expected1{1.2f};
	glm::vec2 expected2{3.4f, 5.6f};
	glm::vec3 expected3{7.8f, 9.10f, 11.12f};
	glm::vec4 expected4{13.14f, 15.16f, 17.18f, 19.20f};

	ASSERT_FLOAT_EQ(expected1, test_ent_float_comp->scalar());

	ASSERT_FLOAT_EQ(expected2.x, test_ent_float_comp->vec2().x);
	ASSERT_FLOAT_EQ(expected2.y, test_ent_float_comp->vec2().y);

	ASSERT_FLOAT_EQ(expected3.x, test_ent_float_comp->vec3().x);
	ASSERT_FLOAT_EQ(expected3.y, test_ent_float_comp->vec3().y);
	ASSERT_FLOAT_EQ(expected3.z, test_ent_float_comp->vec3().z);

	ASSERT_FLOAT_EQ(expected4.x, test_ent_float_comp->vec4().x);
	ASSERT_FLOAT_EQ(expected4.y, test_ent_float_comp->vec4().y);
	ASSERT_FLOAT_EQ(expected4.z, test_ent_float_comp->vec4().z);
	ASSERT_FLOAT_EQ(expected4.w, test_ent_float_comp->vec4().w);
}

TEST(entity_entity_component_test, entity_component_property_float) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_b_float{"
						"scalar=1.2;"
						"vec2=(3.4,5.6);"
						"vec3=(7.8,9.10,11.12);"
						"vec4=(13.14,15.16,17.18,19.20);"
						"}}"
						"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
	entity_component_property_float_verify(em);
}

TEST(entity_entity_component_test, entity_component_property_float_serialize_deserialize) {
	bstream::vector_iobstream stream;
	test_b_system tbsys;
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf{test_b_float{"
							"scalar=1.2;"
							"vec2=(3.4,5.6);"
							"vec3=(7.8,9.10,11.12);"
							"vec4=(13.14,15.16,17.18,19.20);"
							"}}"
							"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_bstream(stream);
		entity_component_property_float_verify(em);
	}
}

TEST(entity_entity_component_test, entity_component_property_float_scalar_as_1d_vector) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_b_float{"
						"scalar=(1.2);"
						"vec2=(3.4,5.6);"
						"vec3=(7.8,9.10,11.12);"
						"vec4=(13.14,15.16,17.18,19.20);"
						"}}"
						"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
	entity_component_property_float_verify(em);
}

static void entity_component_property_int_verify(entity_manager& em) {
	auto test_ent = em.find_entity("test_ent1");
	ASSERT_TRUE(test_ent);
	auto test_ent_int_comp = test_ent->component<test_b_int_component>();
	ASSERT_TRUE(test_ent_int_comp);
	int expected1{123};
	glm::ivec2 expected2{456, 789};
	glm::ivec3 expected3{101112, 131415, 161718};
	glm::ivec4 expected4{192021, 222324, 252627, 282930};

	ASSERT_EQ(expected1, test_ent_int_comp->scalar());

	ASSERT_EQ(expected2.x, test_ent_int_comp->vec2().x);
	ASSERT_EQ(expected2.y, test_ent_int_comp->vec2().y);

	ASSERT_EQ(expected3.x, test_ent_int_comp->vec3().x);
	ASSERT_EQ(expected3.y, test_ent_int_comp->vec3().y);
	ASSERT_EQ(expected3.z, test_ent_int_comp->vec3().z);

	ASSERT_EQ(expected4.x, test_ent_int_comp->vec4().x);
	ASSERT_EQ(expected4.y, test_ent_int_comp->vec4().y);
	ASSERT_EQ(expected4.z, test_ent_int_comp->vec4().z);
	ASSERT_EQ(expected4.w, test_ent_int_comp->vec4().w);
}

TEST(entity_entity_component_test, entity_component_property_int) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_b_int{"
						"scalar=123;"
						"vec2=(456, 789);"
						"vec3=(101112, 131415, 161718);"
						"vec4=(192021, 222324, 252627, 282930);"
						"}}"
						"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
	entity_component_property_int_verify(em);
}

TEST(entity_entity_component_test, entity_component_property_int_serialize_deserialize) {
	bstream::vector_iobstream stream;
	test_b_system tbsys;
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
				"test.etf", "Test_Ent_Conf{test_b_int{"
							"scalar=123;"
							"vec2=(456, 789);"
							"vec3=(101112, 131415, 161718);"
							"vec4=(192021, 222324, 252627, 282930);"
							"}}"
							"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
		em.store_entities_to_bstream(stream);
	}
	{
		entity_manager em(nullptr);
		tbsys.register_with_manager(em);
		em.load_entities_from_bstream(stream);
		entity_component_property_int_verify(em);
	}
}

TEST(entity_entity_component_test, entity_component_property_int_scalar_as_1d_vector) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_b_int{"
						"scalar=(123);"
						"vec2=(456, 789);"
						"vec3=(101112, 131415, 161718);"
						"vec4=(192021, 222324, 252627, 282930);"
						"}}"
						"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
	entity_component_property_int_verify(em);
}

static void entity_component_property_direct_prop_verify(entity_manager& em) {
	auto test_ent = em.find_entity("test_ent1");
	ASSERT_TRUE(test_ent);
	auto test_ent_direct_prop_comp = test_ent->component<test_b_direct_prop_component>();
	ASSERT_TRUE(test_ent_direct_prop_comp);

	ASSERT_EQ(12345, test_ent_direct_prop_comp->get_int_val());
	ASSERT_EQ("Hello World", test_ent_direct_prop_comp->get_string_val());
}

TEST(entity_entity_component_test, entity_component_property_direct_prop) {
	test_b_system tbsys;
	entity_manager em(nullptr);
	tbsys.register_with_manager(em);
	em.load_entities_from_template_lang_file(asset::dummy_asset::create_dummy_asset(
			"test.etf", "Test_Ent_Conf{test_b_direct_prop{"
						"int_val=12345;"
						"string_val=\"Hello World\";"
						"}}"
						"Test_Ent_Conf test_ent1 (0,0,0),(x:90,y:0,z:0);"));
	entity_component_property_direct_prop_verify(em);
}

} // namespace entity
} // namespace mce
