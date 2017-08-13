/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <glm/vec2.hpp>
#include <mce/graphics/graphics_test.hpp>

namespace mce {
namespace graphics {

graphics_test::graphics_test()
		: glfw_win_("Vulkan Test", glm::ivec2(800, 600)), dev_(inst_), win_(inst_, glfw_win_, dev_),
		  mem_mgr_(&dev_, 1 << 27), dqm_(&dev_, uint32_t(win_.swapchain_images().size())),
		  tmgr_(dev_, mem_mgr_, uint32_t(win_.swapchain_images().size())),
		  gmgr_(dev_, &dqm_), last_frame_t_{std::chrono::high_resolution_clock::now()} {
	fbcfg_ = gmgr_.create_framebuffer_config("test", win_, {});
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
