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
	ASSERT_EQ(sizeof(model::model_vertex::position) + sizeof(model::model_vertex::normal) +
					  sizeof(model::model_vertex::tex_coords),
			  sizeof(model::model_vertex));
	ASSERT_EQ(sizeof(model::model_vertex::position) + sizeof(model::model_vertex::normal) +
					  sizeof(model::model_vertex::tex_coords),
			  conf.bindings().at(0).stride);
	ASSERT_EQ(0, conf.bindings().at(0).binding);
	ASSERT_EQ(3, conf.attributes().size());
	ASSERT_EQ(0, conf.attributes().at(0).binding);
	ASSERT_EQ(0, conf.attributes().at(1).binding);
	ASSERT_EQ(0, conf.attributes().at(2).binding);
	ASSERT_EQ(0, conf.attributes().at(0).location);
	ASSERT_EQ(1, conf.attributes().at(1).location);
	ASSERT_EQ(2, conf.attributes().at(2).location);
	ASSERT_EQ(offsetof(model::model_vertex, position), conf.attributes().at(0).offset);
	ASSERT_EQ(offsetof(model::model_vertex, normal), conf.attributes().at(1).offset);
	ASSERT_EQ(offsetof(model::model_vertex, tex_coords), conf.attributes().at(2).offset);
	ASSERT_TRUE(conf.attributes().at(0).offset < conf.attributes().at(1).offset);
	ASSERT_TRUE(conf.attributes().at(1).offset < conf.attributes().at(2).offset);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(0).format);
	ASSERT_EQ(vk::Format::eR32G32B32Sfloat, conf.attributes().at(1).format);
	ASSERT_EQ(vk::Format::eR32G32Sfloat, conf.attributes().at(2).format);
}

} // namespace rendering
} // namespace mce
