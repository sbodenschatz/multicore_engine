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
		  plc_(dev_), mem_mgr_(&dev_, 1 << 27), dqm_(&dev_, uint32_t(win_.swapchain_images().size())),
		  tmgr_(dev_, mem_mgr_, uint32_t(win_.swapchain_images().size())) {}

graphics_test::~graphics_test() {}

void graphics_test::run() {
	//for(int frame=0;frame<4;++frame){
	while(!glfw_win_.should_close()) {
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
