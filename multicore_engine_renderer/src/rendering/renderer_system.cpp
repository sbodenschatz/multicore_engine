/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/engine.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/rendering/renderer_system.hpp>

namespace mce {
namespace rendering {

renderer_system::renderer_system(core::engine& eng, graphics::graphics_system& gs)
		: gs_{gs}, mdl_mgr(eng.model_data_manager(), gs.device(), gs.memory_manager(),
						   &(gs.destruction_queue_manager()), gs.transfer_manager()),
		  mat_mgr(eng.asset_manager(), gs.texture_manager()) {
	gs_.graphics_manager().create_descriptor_set_layout(
			"per_scene",
			{graphics::descriptor_set_layout_binding_element{
					0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics, {}}});
	gs_.graphics_manager().create_descriptor_set_layout(
			"per_material",
			{graphics::descriptor_set_layout_binding_element{0, // Albedo
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {/*TODO*/}},
			 graphics::descriptor_set_layout_binding_element{1, // Normal map
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {/*TODO*/}},
			 graphics::descriptor_set_layout_binding_element{2, // Material
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {/*TODO*/}},
			 graphics::descriptor_set_layout_binding_element{3, // Emission
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {/*TODO*/}}});
	/* Currently unneeded because the model matrix can be passed using push constants.
	 gs_.graphics_manager().create_descriptor_set_layout(
			"per_scene",
			{{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics, {}}});
	*/
}

renderer_system::~renderer_system() {}

void renderer_system::prerender(const mce::core::frame_time&) {}
void renderer_system::postrender(const mce::core::frame_time&) {}

} /* namespace rendering */
} /* namespace mce */
