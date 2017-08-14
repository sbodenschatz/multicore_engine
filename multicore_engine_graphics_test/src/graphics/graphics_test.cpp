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
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_test.hpp>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/graphics/render_pass.hpp>

namespace mce {
namespace graphics {

boost::filesystem::path graphics_test::exe_path(".");

graphics_test::graphics_test()
		: glfw_win_("Vulkan Test", glm::ivec2(800, 600)), dev_(inst_), win_(inst_, glfw_win_, dev_),
		  mem_mgr_(&dev_, 1 << 27), dqm_(&dev_, uint32_t(win_.swapchain_images().size())),
		  tmgr_(dev_, mem_mgr_, uint32_t(win_.swapchain_images().size())),
		  gmgr_(dev_, &dqm_), last_frame_t_{std::chrono::high_resolution_clock::now()} {
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
										   vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore,
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
	pcfg->color_blend_state() = pipeline_config::color_blend_state_config{{{}}};
	pcfg->layout(pll_);
	pcfg->compatible_render_pass(rp_);
	pcfg->compatible_subpass(0);
	gmgr_.add_pending_pipeline("test_pl", pcfg);
	gmgr_.compile_pending_pipelines();
}

graphics_test::~graphics_test() {}

void graphics_test::run() {
	static int frame_counter = 0;
	// for(int frame=0;frame<4;++frame){
	while(!glfw_win_.should_close()) {
		auto this_frame_t = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta_t = this_frame_t - last_frame_t_;
		frame_counter++;
		if(delta_t.count() >= 1.0f) {
			last_frame_t_ = this_frame_t;
			glfw_win_.title("Vulkan Test " + std::to_string(frame_counter / delta_t.count()) + " fps");
			frame_counter = 0;
		}
		/// TODO Use index from acquire.
		tmgr_.start_frame();
		dqm_.advance();
		/// TODO Use buffers
		tmgr_.retrieve_ready_ownership_transfers();

		tmgr_.end_frame();
		glfw_inst_.poll_events();
	}
}

} /* namespace graphics */
} /* namespace mce */
