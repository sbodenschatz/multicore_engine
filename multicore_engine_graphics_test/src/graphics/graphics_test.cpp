/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <glm/vec2.hpp>
#include <iomanip>
#include <iostream>
#include <mce/asset/load_unit_asset_loader.hpp>
#include <mce/asset/pack_file_reader.hpp>
#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_test.hpp>
#include <mce/graphics/pipeline.hpp>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/util/array_utils.hpp>

namespace mce {
namespace graphics {

boost::filesystem::path graphics_test::exe_path(".");

graphics_test::graphics_test()
		: glfw_win_("Vulkan Test", glm::ivec2(800, 600)), dev_(inst_), win_(inst_, glfw_win_, dev_),
		  mem_mgr_(&dev_, 1 << 27), render_cmd_pool_(dev_, dev_.graphics_queue_index().first, true),
		  dqm_(&dev_, uint32_t(win_.swapchain_images().size())),
		  tmgr_(dev_, mem_mgr_, uint32_t(win_.swapchain_images().size())), gmgr_(dev_, &dqm_),
		  tmp_semaphore_(dev_->createSemaphoreUnique({})),
		  acquire_semaphores_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return dev_->createSemaphoreUnique({});
							  })),
		  present_semaphores_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return dev_->createSemaphoreUnique({});
							  })),
		  fences_(win_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return dev_->createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
				  })),
		  vertex_buffer_(dev_, mem_mgr_, &dqm_, sizeof(vertex) * 3,
						 vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
						 vk::MemoryPropertyFlagBits::eDeviceLocal),
		  last_frame_t_{std::chrono::high_resolution_clock::now()} {
	fbcfg_ = gmgr_.create_framebuffer_config("test_fbcfg", win_, {});
	pll_ = gmgr_.create_pipeline_layout("test_pll", {}, {});
	spg_ = gmgr_.create_subpass_graph(
			"test_spg",
			{subpass_entry{
					{}, {vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)}, {}, {}, {}}},
			{});
	rp_ = gmgr_.create_render_pass(
			"test_rp", spg_, fbcfg_,
			{render_pass_attachment_access{vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,
										   vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
										   vk::AttachmentLoadOp::eDontCare,
										   vk::AttachmentStoreOp::eDontCare}});
	auto loader = std::make_shared<asset::load_unit_asset_loader>(std::vector<asset::path_prefix>(
			{{std::make_unique<asset::pack_file_reader>(), "assets.pack"},
			 {std::make_unique<asset::pack_file_reader>(), "../multicore_engine_assets/assets.pack"},
			 {std::make_unique<asset::pack_file_reader>(),
			  ((exe_path.parent_path() / "multicore_engine_assets") / "assets.pack").string()}}));
	amgr_.add_asset_loader(loader);
	amgr_.start_pin_load_unit("shaders");
	auto vert_shader_bin = amgr_.load_asset_sync("shaders/test_shader.vert.spv");
	for(size_t i = 0; i < vert_shader_bin->size(); ++i) {
		std::cout << std::hex << std::setw(2) << std::setfill('0')
				  << int(reinterpret_cast<const unsigned char*>(vert_shader_bin->data())[i]) << " ";
		if(i % 32 == 31) std::cout << std::endl;
	}
	vert_shader_ = gmgr_.create_shader_module("test_vert_shader", *vert_shader_bin);
	auto frag_shader_bin = amgr_.load_asset_sync("shaders/test_shader.frag.spv");
	frag_shader_ = gmgr_.create_shader_module("test_frag_shader", *frag_shader_bin);
	auto pcfg = std::make_shared<pipeline_config>();
	pcfg->shader_stages() = {{vk::ShaderStageFlagBits::eVertex, vert_shader_, "main"},
							 {vk::ShaderStageFlagBits::eFragment, frag_shader_, "main"}};
	pcfg->input_state() = {{{0, sizeof(vertex)}}, {{0, 0, vk::Format::eR32G32Sfloat, offsetof(vertex, pos)}}};
	pcfg->assembly_state() = {{}, vk::PrimitiveTopology::eTriangleList};
	pcfg->viewport_state() = pipeline_config::viewport_state_config{
			{{0.0, 0.0, float(win_.swapchain_size().x), float(win_.swapchain_size().y), 0.0f, 1.0f}},
			{{{0, 0}, {win_.swapchain_size().x, win_.swapchain_size().y}}}};
	pcfg->rasterization_state().lineWidth = 1.0f;
	pcfg->multisample_state() = vk::PipelineMultisampleStateCreateInfo{};
	vk::PipelineColorBlendAttachmentState pcbas;
	pcbas.colorWriteMask = ~vk::ColorComponentFlags();
	pcfg->color_blend_state() = pipeline_config::color_blend_state_config{{pcbas}};
	pcfg->layout(pll_);
	pcfg->compatible_render_pass(rp_);
	pcfg->compatible_subpass(0);
	gmgr_.add_pending_pipeline("test_pl", pcfg);
	gmgr_.compile_pending_pipelines();
	plc_ = gmgr_.find_pipeline_config("test_pl");
	pl_ = gmgr_.find_pipeline("test_pl");
	fb_ = std::make_unique<framebuffer>(dev_, win_, mem_mgr_, &dqm_, fbcfg_, rp_->native_render_pass());
	vertex vertices[] = {{{0.0f, -1.0f}}, {{-1.0f, 1.0f}}, {{1.0f, 1.0f}}};
	tmgr_.upload_buffer(vertices, sizeof(vertices), vertex_buffer_.native_buffer(), 0,
						[this](vk::Buffer) { //
							vb_ready_ = true;
						});
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
		frame_counter++;
		if(delta_t.count() >= 1.0f) {
			last_frame_t_ = this_frame_t;
			glfw_win_.title("Vulkan Test " + std::to_string(frame_counter / delta_t.count()) + " fps");
			frame_counter = 0;
		}
		tmgr_.start_frame(acq_res.value);
		dqm_.cleanup_and_set_current(acq_res.value);
		auto render_cmb_buf = queued_handle<vk::UniqueCommandBuffer>(
				render_cmd_pool_.allocate_primary_command_buffer(), &dqm_);
		render_cmb_buf->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		vk::ClearValue clear(vk::ClearColorValue(util::make_array<float>(0.0f, 1.0f, 0.0f, 1.0f)));
		render_cmb_buf->beginRenderPass(
				vk::RenderPassBeginInfo(
						rp_->native_render_pass(), fb_->frames()[img_index].native_framebuffer(),
						vk::Rect2D({0, 0}, {win_.swapchain_size().x, win_.swapchain_size().y}), 1, &clear),
				vk::SubpassContents::eInline);
		if(vb_ready_) {
			pl_->bind(render_cmb_buf.get());
			render_cmb_buf->bindVertexBuffers(0, vertex_buffer_.native_buffer(), vk::DeviceSize(0));
			render_cmb_buf->draw(3, 1, 0, 0);
		}
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
