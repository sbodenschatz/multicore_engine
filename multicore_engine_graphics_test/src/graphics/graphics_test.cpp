/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <iomanip>
#include <iostream>
#include <mce/asset/load_unit_asset_loader.hpp>
#include <mce/asset/pack_file_reader.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_test.hpp>
#include <mce/graphics/pipeline.hpp>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/sampler.hpp>
#include <mce/graphics/shader_loader.hpp>
#include <mce/model/dump_model.hpp>
#include <mce/rendering/model_format.hpp>
#include <mce/util/array_utils.hpp>

namespace mce {
namespace graphics {

boost::filesystem::path graphics_test::exe_path(".");

graphics_test::graphics_test()
		: mdmgr(amgr_), glfw_win_("Vulkan Test", glm::ivec2(800, 600)), dev_(inst_),
		  win_(inst_, glfw_win_, dev_), mem_mgr_(&dev_, 1 << 27),
		  render_cmd_pool_(dev_, dev_.graphics_queue_index().first, true),
		  dqm_(&dev_, uint32_t(win_.swapchain_images().size())),
		  tmgr_(dev_, mem_mgr_, uint32_t(win_.swapchain_images().size())),
		  mmgr_(mdmgr, dev_, mem_mgr_, &dqm_, tmgr_), tex_mgr_(amgr_, dev_, mem_mgr_, &dqm_, tmgr_),
		  mat_mgr_(amgr_, tex_mgr_), gmgr_(dev_, &dqm_), tmp_semaphore_(dev_->createSemaphoreUnique({})),
		  acquire_semaphores_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return dev_->createSemaphoreUnique({});
							  })),
		  present_semaphores_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return dev_->createSemaphoreUnique({});
							  })),
		  fences_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return dev_->createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
				  })),
		  uniform_buffers_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
							   return simple_uniform_buffer(dev_, mem_mgr_, &dqm_, 4096);
						   })),
		  vertex_buffer_(dev_, mem_mgr_, &dqm_, sizeof(vertex) * 6,
						 vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
						 vk::MemoryPropertyFlagBits::eDeviceLocal),
		  last_frame_t_{std::chrono::high_resolution_clock::now()} {
	fbcfg_ = gmgr_.create_framebuffer_config(
			"test_fbcfg", win_,
			{framebuffer_attachment_config(dev_.best_supported_depth_attachment_format(),
										   image_aspect_mode::depth)},
			{framebuffer_pass_config({0, 1})});
	sampler_ = gmgr_.create_sampler("test_sampler", vk::Filter::eLinear, vk::Filter::eLinear,
									vk::SamplerMipmapMode::eLinear,
									sampler_addressing_mode(vk::SamplerAddressMode::eRepeat), 0.0f, 16.0f, {},
									0.0f, 7.0f, vk::BorderColor::eIntOpaqueBlack);
	uniform_dsl_ = gmgr_.create_descriptor_set_layout(
			"test_uniform_dsl",
			{descriptor_set_layout_binding_element{
					 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, {}},
			 descriptor_set_layout_binding_element{1,
												   vk::DescriptorType::eCombinedImageSampler,
												   1,
												   vk::ShaderStageFlagBits::eFragment,
												   {sampler_->native_sampler()}}});
	descriptor_pools_ = {win_.swapchain_images().size(), containers::generator_param([this](size_t) {
							 return simple_descriptor_pool(dev_, descriptor_set_resources(*uniform_dsl_, 16));
						 })};
	pll_ = gmgr_.create_pipeline_layout("test_pll", {uniform_dsl_}, {});
	spg_ = gmgr_.create_subpass_graph(
			"test_spg",
			{subpass_entry{{},
						   {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)},
						   {},
						   vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal),
						   {}}},
			{});
	rp_ = gmgr_.create_render_pass(
			"test_rp", spg_, fbcfg_, 0,
			{render_pass_attachment_access{vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,
										   vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
										   vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare},
			 render_pass_attachment_access{
					 vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal,
					 vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
					 vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare}});
	auto loader = std::make_shared<asset::load_unit_asset_loader>(std::vector<asset::path_prefix>(
			{{std::make_unique<asset::pack_file_reader>(), "assets.pack"},
			 {std::make_unique<asset::pack_file_reader>(), "../multicore_engine_assets/assets.pack"},
			 {std::make_unique<asset::pack_file_reader>(),
			  ((exe_path.parent_path() / "multicore_engine_assets") / "assets.pack").string()}}));
	amgr_.add_asset_loader(loader);
	amgr_.start_pin_load_unit("shaders");
	amgr_.start_pin_load_unit("models_geo");

	shader_loader shader_ldr(amgr_, gmgr_);
	shader_ldr.load_shader("shaders/test_shader.vert");
	shader_ldr.load_shader("shaders/test_shader.frag");
	shader_ldr.load_shader("shaders/test_shader2.vert");
	shader_ldr.load_shader("shaders/test_shader2.frag");
	shader_ldr.wait_for_completion();
	vert_shader_ = gmgr_.find_shader_module("shaders/test_shader.vert");
	frag_shader_ = gmgr_.find_shader_module("shaders/test_shader.frag");
	vert_shader2_ = gmgr_.find_shader_module("shaders/test_shader2.vert");
	frag_shader2_ = gmgr_.find_shader_module("shaders/test_shader2.frag");

	auto pcfg = std::make_shared<pipeline_config>();
	pcfg->shader_stages() = {{vk::ShaderStageFlagBits::eVertex, vert_shader_, "main"},
							 {vk::ShaderStageFlagBits::eFragment, frag_shader_, "main"}};
	pcfg->input_state() = {{{0, sizeof(vertex)}},
						   {{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(vertex, pos)},
							{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(vertex, color)}}};
	pcfg->assembly_state() = {{}, vk::PrimitiveTopology::eTriangleList};
	pcfg->viewport_state() = pipeline_config::viewport_state_config{
			{{0.0, 0.0, float(win_.swapchain_size().x), float(win_.swapchain_size().y), 0.0f, 1.0f}},
			{{{0, 0}, {win_.swapchain_size().x, win_.swapchain_size().y}}}};
	pcfg->rasterization_state().lineWidth = 1.0f;
	pcfg->multisample_state() = vk::PipelineMultisampleStateCreateInfo{};
	pcfg->depth_stencil_state() =
			vk::PipelineDepthStencilStateCreateInfo({}, true, true, vk::CompareOp::eLess);
	vk::PipelineColorBlendAttachmentState pcbas;
	pcbas.colorWriteMask = ~vk::ColorComponentFlags();
	pcfg->color_blend_state() = pipeline_config::color_blend_state_config{{pcbas}};
	pcfg->layout(pll_);
	pcfg->compatible_render_pass(rp_);
	pcfg->compatible_subpass(0);
	gmgr_.add_pending_pipeline("test_pl", pcfg);

	auto pcfg2 = std::make_shared<pipeline_config>();
	pcfg2->shader_stages() = {{vk::ShaderStageFlagBits::eVertex, vert_shader2_, "main"},
							  {vk::ShaderStageFlagBits::eFragment, frag_shader2_, "main"}};
	pcfg2->input_state() = rendering::model_vertex_input_config();
	pcfg2->assembly_state() = {{}, vk::PrimitiveTopology::eTriangleList};
	pcfg2->viewport_state() = pipeline_config::viewport_state_config{
			{{0.0, 0.0, float(win_.swapchain_size().x), float(win_.swapchain_size().y), 0.0f, 1.0f}},
			{{{0, 0}, {win_.swapchain_size().x, win_.swapchain_size().y}}}};
	pcfg2->rasterization_state().lineWidth = 1.0f;
	pcfg2->multisample_state() = vk::PipelineMultisampleStateCreateInfo{};
	pcfg2->depth_stencil_state() =
			vk::PipelineDepthStencilStateCreateInfo({}, true, true, vk::CompareOp::eLess);
	vk::PipelineColorBlendAttachmentState pcbas2;
	pcbas2.colorWriteMask = ~vk::ColorComponentFlags();
	pcfg2->color_blend_state() = pipeline_config::color_blend_state_config{{pcbas2}};
	pcfg2->layout(pll_);
	pcfg2->compatible_render_pass(rp_);
	pcfg2->compatible_subpass(0);
	gmgr_.add_pending_pipeline("test_pl2", pcfg2);

	gmgr_.compile_pending_pipelines();
	plc_ = gmgr_.find_pipeline_config("test_pl");
	pl_ = gmgr_.find_pipeline("test_pl");
	plc2_ = gmgr_.find_pipeline_config("test_pl2");
	pl2_ = gmgr_.find_pipeline("test_pl2");
	fb_ = std::make_unique<framebuffer>(dev_, win_, mem_mgr_, &dqm_, fbcfg_,
										std::vector<vk::RenderPass>({rp_->native_render_pass()}));
	vertex vertices[] = {{{0.0f, -1.0f, 0.2f}, {1.0f, 0.0f, 0.0f}}, {{-1.0f, 1.0f, 0.2f}, {0.0f, 1.0f, 0.0f}},
						 {{1.0f, 1.0f, 0.2f}, {0.0f, 0.0f, 1.0f}},

						 {{0.0f, 1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},  {{1.0f, -1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}},
						 {{-1.0f, -1.0f, 0.1f}, {1.0f, 0.0f, 0.0f}}};
	tmgr_.upload_buffer(vertices, sizeof(vertices), vertex_buffer_.native_buffer(), 0,
						[this](vk::Buffer) { //
							vb_ready_ = true;
						});
	mdl_ = mmgr_.load_static_model(
			"models/cube",
			[](rendering::static_model_ptr mdl) { //
				std::cout << mdl->meshes().size() << std::endl;
				model::dump_model_vertices(std::cout, mdl->poly_model()->content_data(),
										   reinterpret_cast<const char*>(mdl->poly_model()->content_data()) +
												   mdl->meshes().at(0).indices_offset(),
										   mdl->meshes().at(0).vertex_count());
			},
			[](std::exception_ptr) {});
	mat_mgr_.load_material_library("materials/test");
	mat_ = mat_mgr_.load_material("test");
}

graphics_test::~graphics_test() {
	containers::dynamic_array<vk::Fence> tmp(
			fences_.size(), containers::generator_param([this](size_t i) { return fences_[i].get(); }));
	dev_->waitForFences({uint32_t(tmp.size()), tmp.data()}, true, ~0u);
}

void graphics_test::run() {
	static int frame_counter = 0;
	std::vector<vk::CommandBuffer> cmd_buff_handles;
	// for(int frame=0;frame<4;++frame){
	while(!glfw_win_.should_close()) {
		auto acq_res = dev_->acquireNextImageKHR(win_.swapchain(), ~0ull, tmp_semaphore_.get(), vk::Fence());
		auto img_index = acq_res.value;
		dev_->waitForFences(fences_[img_index].get(), true, ~0u);
		dev_->resetFences(fences_[img_index].get());
		using std::swap;
		swap(tmp_semaphore_, acquire_semaphores_[acq_res.value]);
		glfw_inst_.poll_events();
		auto this_frame_t = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta_t = this_frame_t - last_frame_t_;
		last_frame_t_ = this_frame_t;
		rot_angle += glm::vec3(1.0f, 0.7f, 0.5f) * glm::radians(30.0f) * delta_t.count();
		frame_counter++;
		std::chrono::duration<float> delta_t_fps = this_frame_t - last_fps_frame_t_;
		if(delta_t_fps.count() >= 1.0f) {
			last_fps_frame_t_ = this_frame_t;
			glfw_win_.title("Vulkan Test " + std::to_string(frame_counter / delta_t_fps.count()) + " fps");
			frame_counter = 0;
		}
		tmgr_.start_frame(acq_res.value);
		dqm_.cleanup_and_set_current(acq_res.value);
		uniform_buffers_[img_index].reset();
		descriptor_pools_[img_index].reset();
		uniform_data ud;
		ud.projection = glm::perspectiveFov(glm::radians(90.0f), float(win_.swapchain_size().x),
											float(win_.swapchain_size().y), 1.0f, 1000.0f);
		ud.model = glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(), {0.0f, 0.0f, -4.0f}),
													   rot_angle.x, {1.0f, 0.0f, 0.0f}),
										   rot_angle.y, {0.0f, 1.0f, 0.0f}),
							   rot_angle.z, {0.0f, 0.0f, 1.0f});
		auto render_cmb_buf = queued_handle<vk::UniqueCommandBuffer>(
				render_cmd_pool_.allocate_primary_command_buffer(), &dqm_);
		render_cmb_buf->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		vk::ClearValue clear[] = {
				vk::ClearColorValue(util::make_array<float>(100 / 255.0f, 149 / 255.0f, 237 / 255.0f, 1.0f)),
				vk::ClearDepthStencilValue(1.0f)};
		render_cmb_buf->beginRenderPass(
				vk::RenderPassBeginInfo(
						rp_->native_render_pass(),
						fb_->passes().at(0).frames()[img_index].native_framebuffer(),
						vk::Rect2D({0, 0}, {win_.swapchain_size().x, win_.swapchain_size().y}), 2, clear),
				vk::SubpassContents::eInline);
		if(mdl_->ready() && mat_->ready()) {
			auto ds = descriptor_pools_[img_index].allocate_descriptor_set(uniform_dsl_);
			ds.update()(0, 0, vk::DescriptorType::eUniformBuffer, uniform_buffers_[img_index].store(ud))(
					1, 0, vk::DescriptorType::eCombinedImageSampler,
					mat_->albedo_map()->bind(/*sampler_.get()*/));
			ds.bind(render_cmb_buf.get(), *pll_, 0, ds);
			pl2_->bind(render_cmb_buf.get());
			mdl_->draw_model_mesh(render_cmb_buf.get(), 0);
		}
		/*if(vb_ready_) {
			pl_->bind(render_cmb_buf.get());
			render_cmb_buf->bindVertexBuffers(0, vertex_buffer_.native_buffer(), vk::DeviceSize(0));
			render_cmb_buf->draw(6, 1, 0, 0);
		}*/
		render_cmb_buf->endRenderPass();
		render_cmb_buf->pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands,
				vk::PipelineStageFlagBits::eHost | vk::PipelineStageFlagBits::eAllCommands, {},
				{vk::MemoryBarrier(~vk::AccessFlags(), ~vk::AccessFlags())}, {}, {});
		render_cmb_buf->end();
		auto cmd_buffers = tmgr_.retrieve_ready_ownership_transfers();
		cmd_buffers.push_back(std::move(render_cmb_buf));
		cmd_buff_handles.clear();
		std::transform(cmd_buffers.begin(), cmd_buffers.end(), std::back_inserter(cmd_buff_handles),
					   [](const auto& h) { return h.get(); });
		auto acq_sema = acquire_semaphores_[img_index].get();
		vk::PipelineStageFlags wait_ps = vk::PipelineStageFlagBits::eTopOfPipe;
		auto present_sema = present_semaphores_[img_index].get();
		uniform_buffers_[img_index].flush();
		dev_.graphics_queue().submit(
				{vk::SubmitInfo(1, &acq_sema, &wait_ps, uint32_t(cmd_buff_handles.size()),
								cmd_buff_handles.data(), 1, &present_sema)},
				fences_[img_index].get());
		auto swapchain_handle = win_.swapchain();
		dev_.present_queue().presentKHR(
				vk::PresentInfoKHR(1, &present_sema, 1, &swapchain_handle, &img_index));
		tmgr_.end_frame();
	}
}

} /* namespace graphics */
} /* namespace mce */
