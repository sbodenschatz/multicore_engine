/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/sampler.hpp>
#include <mce/graphics/shader_loader.hpp>
#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/uniforms_structs.hpp>

namespace mce {
namespace rendering {

renderer_system::renderer_system(core::engine& eng, graphics::graphics_system& gs)
		: eng_{eng}, gs_{gs}, mdl_mgr(eng.model_data_manager(), gs.device(), gs.memory_manager(),
									  &(gs.destruction_queue_manager()), gs.transfer_manager()),
		  mat_mgr(eng.asset_manager(), gs.texture_manager()) {

	graphics::shader_loader shader_ldr(eng.asset_manager(), gs_.graphics_manager());
	std::string main_forward_vertex_shader_name = "shaders/main_forward.vert";
	shader_ldr.load_shader(main_forward_vertex_shader_name);
	std::string main_forward_fragment_shader_name = "shaders/main_forward.frag";
	shader_ldr.load_shader(main_forward_fragment_shader_name);

	create_samplers();
	create_descriptor_sets();
	pipeline_layout_scene_pass_ = gs_.graphics_manager().create_pipeline_layout(
			"scene_pass", {descriptor_set_layout_per_scene_, descriptor_set_layout_per_material_},
			{vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0,
								   sizeof(per_object_push_constants))});
	auto main_fbcfg = gs_.graphics_manager().create_framebuffer_config(
			"main_fbcfg", gs_.window(),
			{graphics::framebuffer_attachment_config(gs_.device().best_supported_depth_attachment_format(),
													 graphics::image_aspect_mode::depth)},
			{graphics::framebuffer_pass_config({0, 1})});
	auto main_spg = gs_.graphics_manager().create_subpass_graph(
			"main_spg",
			{graphics::subpass_entry{
					{},
					{vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)},
					{},
					vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal),
					{}}},
			{});
	main_render_pass_ = gs.graphics_manager().create_render_pass(
			"main_rp", main_spg, main_fbcfg, 0,
			{graphics::render_pass_attachment_access{
					 vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
					 vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
					 vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare},
			 graphics::render_pass_attachment_access{
					 vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal,
					 vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
					 vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare}});
	main_framebuffer_ = std::make_unique<graphics::framebuffer>(
			gs_.device(), gs_.window(), gs_.memory_manager(), &(gs_.destruction_queue_manager()), main_fbcfg,
			std::vector<vk::RenderPass>{main_render_pass_->native_render_pass()});
	shader_ldr.wait_for_completion();
}

renderer_system::~renderer_system() {}

void renderer_system::prerender(const mce::core::frame_time&) {}
void renderer_system::postrender(const mce::core::frame_time&) {}

void renderer_system::create_samplers() {
	auto anisotropy = eng_.config_store().resolve("anisotropy", -1.0f);
	boost::optional<float> max_anisotropy;
	auto aniso_val = anisotropy->value();
	if(aniso_val > 0.0f) {
		max_anisotropy = aniso_val;
	}
	default_sampler_ = gs_.graphics_manager().create_sampler(
			"default", vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
			graphics::sampler_addressing_mode(vk::SamplerAddressMode::eRepeat), 0.0f, max_anisotropy, {}, 0.0,
			64.0f, vk::BorderColor::eIntOpaqueBlack);
}

void renderer_system::create_descriptor_sets() {
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
}

} /* namespace rendering */
} /* namespace mce */
