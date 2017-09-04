/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cassert>
#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/sampler.hpp>
#include <mce/graphics/shader_loader.hpp>
#include <mce/rendering/model_format.hpp>
#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/uniforms_structs.hpp>
#include <mce/util/array_utils.hpp>

namespace mce {
namespace rendering {

renderer_system::renderer_system(core::engine& eng, graphics::graphics_system& gs,
								 renderer_system_settings settings)
		: eng_{eng}, gs_{gs}, settings_{std::move(settings)},
		  mdl_mgr(eng.model_data_manager(), gs.device(), gs.memory_manager(),
				  &(gs.destruction_queue_manager()), gs.transfer_manager()),
		  mat_mgr(eng.asset_manager(), gs.texture_manager()),
		  primary_cmd_pool(gs_.device(), gs_.device().graphics_queue_index().first, true, true) {

	graphics::shader_loader shader_ldr(eng.asset_manager(), gs_.graphics_manager());
	shader_ldr.load_shader(settings_.main_forward_vertex_shader_name);
	shader_ldr.load_shader(settings_.main_forward_fragment_shader_name);

	create_samplers();
	create_descriptor_sets();
	create_pipeline_layouts();
	create_render_passes_and_framebuffers();
	shader_ldr.wait_for_completion();
	create_pipelines();
	create_per_frame_data();
}

renderer_system::~renderer_system() {
	gs_.device()->waitIdle();
}

void renderer_system::prerender(const mce::core::frame_time&) {
	auto& pcmdb = per_frame_data_[gs_.current_swapchain_image()].primary_command_buffer;
	// pcmdb->reset({});
	pcmdb->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	std::array<vk::ClearValue, 2> clear = {
			{vk::ClearColorValue(util::make_array<float>(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f)),
			vk::ClearDepthStencilValue(1.0f)}};
	main_render_pass_->begin(pcmdb.get(), main_framebuffer_->pass(0).frame(gs_.current_swapchain_image()),
							 clear, vk::SubpassContents::eSecondaryCommandBuffers);
}
void renderer_system::postrender(const mce::core::frame_time&) {
	auto& pcmdb = per_frame_data_[gs_.current_swapchain_image()].primary_command_buffer;
	pcmdb->endRenderPass();
	pcmdb->end();
}

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

void renderer_system::create_pipeline_layouts() {
	pipeline_layout_scene_pass_ = gs_.graphics_manager().create_pipeline_layout(
			"scene_pass", {descriptor_set_layout_per_scene_, descriptor_set_layout_per_material_},
			{vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0,
								   sizeof(per_object_push_constants))});
}
void renderer_system::create_render_passes_and_framebuffers() {
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
	main_render_pass_ = gs_.graphics_manager().create_render_pass(
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
}

void renderer_system::create_pipelines() {
	auto main_forward_pcfg = std::make_shared<graphics::pipeline_config>();

	auto main_forward_vertex_shader =
			gs_.graphics_manager().find_shader_module(settings_.main_forward_vertex_shader_name);
	assert(main_forward_vertex_shader);
	auto main_forward_fragment_shader =
			gs_.graphics_manager().find_shader_module(settings_.main_forward_fragment_shader_name);
	assert(main_forward_fragment_shader);

	main_forward_pcfg->shader_stages() = {
			{vk::ShaderStageFlagBits::eVertex, main_forward_vertex_shader, "main"},
			{vk::ShaderStageFlagBits::eFragment, main_forward_fragment_shader, "main"}};
	main_forward_pcfg->input_state() = model_vertex_input_config();
	main_forward_pcfg->assembly_state() = {{}, vk::PrimitiveTopology::eTriangleList};
	main_forward_pcfg->viewport_state() = graphics::pipeline_config::viewport_state_config{
			{{0.0, 0.0, float(gs_.window().swapchain_size().x), float(gs_.window().swapchain_size().y), 0.0f,
			  1.0f}},
			{{{0, 0}, {gs_.window().swapchain_size().x, gs_.window().swapchain_size().y}}}};
	main_forward_pcfg->rasterization_state().lineWidth = 1.0f;
	main_forward_pcfg->multisample_state() = vk::PipelineMultisampleStateCreateInfo{};
	main_forward_pcfg->depth_stencil_state() =
			vk::PipelineDepthStencilStateCreateInfo({}, true, true, vk::CompareOp::eLess);
	vk::PipelineColorBlendAttachmentState pcbas_set;
	pcbas_set.colorWriteMask = ~vk::ColorComponentFlags();
	main_forward_pcfg->color_blend_state() = graphics::pipeline_config::color_blend_state_config{{pcbas_set}};
	main_forward_pcfg->layout(pipeline_layout_scene_pass_);
	main_forward_pcfg->compatible_render_pass(main_render_pass_);
	main_forward_pcfg->compatible_subpass(0);
	gs_.graphics_manager().add_pending_pipeline("main_forward_pl", main_forward_pcfg);

	gs_.graphics_manager().compile_pending_pipelines();
	main_forward_pipeline_ = gs_.graphics_manager().find_pipeline("main_forward_pl");
}
void renderer_system::create_per_frame_data() {
	per_frame_data_ = {gs_.window().swapchain_images().size(), containers::generator_param([this](size_t) {
						   return per_frame_data_t{primary_cmd_pool.allocate_primary_command_buffer()};
					   })};
}

} /* namespace rendering */
} /* namespace mce */
