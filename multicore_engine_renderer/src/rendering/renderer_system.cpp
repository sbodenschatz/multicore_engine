/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/graphics/sampler.hpp>
#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/uniforms_structs.hpp>

namespace mce {
namespace rendering {

renderer_system::renderer_system(core::engine& eng, graphics::graphics_system& gs)
		: gs_{gs}, mdl_mgr(eng.model_data_manager(), gs.device(), gs.memory_manager(),
						   &(gs.destruction_queue_manager()), gs.transfer_manager()),
		  mat_mgr(eng.asset_manager(), gs.texture_manager()) {
	auto anisotropy = eng.config_store().resolve("anisotropy", -1.0f);
	boost::optional<float> max_anisotropy;
	auto aniso_val = anisotropy->value();
	if(aniso_val > 0.0f) {
		max_anisotropy = aniso_val;
	}
	default_sampler_ = gs.graphics_manager().create_sampler(
			"default", vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
			graphics::sampler_addressing_mode(vk::SamplerAddressMode::eRepeat), 0.0f, max_anisotropy, {}, 0.0,
			64.0f, vk::BorderColor::eIntOpaqueBlack);
	descriptor_set_layout_per_scene_ = gs_.graphics_manager().create_descriptor_set_layout(
			"per_scene",
			{graphics::descriptor_set_layout_binding_element{
					0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics, {}}});
	descriptor_set_layout_per_material_ = gs_.graphics_manager().create_descriptor_set_layout(
			"per_material",
			{graphics::descriptor_set_layout_binding_element{0, // Albedo
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {default_sampler_->native_sampler()}},
			 graphics::descriptor_set_layout_binding_element{1, // Normal map
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {default_sampler_->native_sampler()}},
			 graphics::descriptor_set_layout_binding_element{2, // Material
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {default_sampler_->native_sampler()}},
			 graphics::descriptor_set_layout_binding_element{3, // Emission
															 vk::DescriptorType::eCombinedImageSampler,
															 1,
															 vk::ShaderStageFlagBits::eFragment,
															 {default_sampler_->native_sampler()}}});
	/* Currently unneeded because the model matrix can be passed using push constants.
	 gs_.graphics_manager().create_descriptor_set_layout(
			"per_scene",
			{{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eAllGraphics, {}}});
	*/
	pipeline_layout_scene_pass_ = gs_.graphics_manager().create_pipeline_layout(
			"scene_pass", {descriptor_set_layout_per_scene_, descriptor_set_layout_per_material_},
			{vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0,
								   sizeof(per_object_push_constants))});
}

renderer_system::~renderer_system() {}

void renderer_system::prerender(const mce::core::frame_time&) {}
void renderer_system::postrender(const mce::core::frame_time&) {}

} /* namespace rendering */
} /* namespace mce */
