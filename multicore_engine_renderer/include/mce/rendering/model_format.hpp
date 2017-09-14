/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/model_format.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MODEL_FORMAT_HPP_
#define MCE_RENDERING_MODEL_FORMAT_HPP_

/**
 * \file
 * Defines helper functions related to the model format used by the renderer.
 */

#include <mce/graphics/format_from_type.hpp>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/model/model_format.hpp>

namespace mce {
namespace rendering {

/// Returns a vertex input state config for the vertices of rendered models (defined in model::model_vertex).
graphics::pipeline_config::vertex_input_state_config model_vertex_input_config() {
	return graphics::pipeline_config::vertex_input_state_config(
			{vk::VertexInputBindingDescription(0, sizeof(model::model_vertex), vk::VertexInputRate::eVertex)},
			{vk::VertexInputAttributeDescription(
					 0, 0,
					 graphics::format_from_type<decltype(model::model_vertex::position)>::default_format,
					 offsetof(model::model_vertex, position)),
			 vk::VertexInputAttributeDescription(
					 1, 0, graphics::format_from_type<decltype(model::model_vertex::tangent)>::default_format,
					 offsetof(model::model_vertex, tangent)),
			 vk::VertexInputAttributeDescription(
					 2, 0,
					 graphics::format_from_type<decltype(model::model_vertex::bitangent)>::default_format,
					 offsetof(model::model_vertex, bitangent)),
			 vk::VertexInputAttributeDescription(
					 3, 0, graphics::format_from_type<decltype(model::model_vertex::normal)>::default_format,
					 offsetof(model::model_vertex, normal)),
			 vk::VertexInputAttributeDescription(
					 4, 0,
					 graphics::format_from_type<decltype(model::model_vertex::tex_coords)>::default_format,
					 offsetof(model::model_vertex, tex_coords))});
}

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_MODEL_FORMAT_HPP_ */
