/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/rendering/model_format_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/rendering/model_format.hpp>

namespace mce {
namespace rendering {

TEST(rendering_model_format_test, expected_vertex_input_config) {
	auto conf = model_vertex_input_config();
	ASSERT_EQ(1, conf.bindings().size());
	auto attributes_size_sum = sizeof(model::model_vertex::position) + sizeof(model::model_vertex::tangent) +
							   sizeof(model::model_vertex::bitangent) + sizeof(model::model_vertex::normal) +
							   sizeof(model::model_vertex::tex_coords);
	ASSERT_EQ(attributes_size_sum, sizeof(model::model_vertex));
	ASSERT_EQ(attributes_size_sum, conf.bindings().at(0).stride);
	ASSERT_EQ(0, conf.bindings().at(0).binding);
	ASSERT_EQ(5, conf.attributes().size());
	ASSERT_EQ(0, conf.attributes().at(0).binding);
	ASSERT_EQ(0, conf.attributes().at(1).binding);
	ASSERT_EQ(0, conf.attributes().at(2).binding);
	ASSERT_EQ(0, conf.attributes().at(3).binding);
	ASSERT_EQ(0, conf.attributes().at(4).binding);
	ASSERT_EQ(0, conf.attributes().at(0).location);
	ASSERT_EQ(1, conf.attributes().at(1).location);
	ASSERT_EQ(2, conf.attributes().at(2).location);
	ASSERT_EQ(3, conf.attributes().at(3).location);
	ASSERT_EQ(4, conf.attributes().at(4).location);
	ASSERT_EQ(offsetof(model::model_vertex, position), conf.attributes().at(0).offset);
	ASSERT_EQ(offsetof(model::model_vertex, tangent), conf.attributes().at(1).offset);
	ASSERT_EQ(offsetof(model::model_vertex, bitangent), conf.attributes().at(2).offset);
	ASSERT_EQ(offsetof(model::model_vertex, normal), conf.attributes().at(3).offset);
	ASSERT_EQ(offsetof(model::model_vertex, tex_coords), conf.attributes().at(4).offset);
	ASSERT_TRUE(conf.attributes().at(0).offset < conf.attributes().at(1).offset);
	ASSERT_TRUE(conf.attributes().at(1).offset < conf.attributes().at(2).offset);
	ASSERT_TRUE(conf.attributes().at(2).offset < conf.attributes().at(3).offset);
	ASSERT_TRUE(conf.attributes().at(3).offset < conf.attributes().at(4).offset);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(0).format);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(1).format);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(2).format);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(3).format);
	ASSERT_EQ(vk::Format::eR32G32Sfloat, conf.attributes().at(4).format);
}

} // namespace rendering
} // namespace mce
