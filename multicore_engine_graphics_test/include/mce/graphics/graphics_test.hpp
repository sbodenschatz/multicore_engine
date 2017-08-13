/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_TEST_HPP_
#define MCE_GRAPHICS_GRAPHICS_TEST_HPP_

#include <chrono>
#include <mce/glfw/instance.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {

class graphics_test {
	glfw::instance glfw_inst_;
	glfw::window glfw_win_;
	instance inst_;
	device dev_;
	window win_;
	device_memory_manager mem_mgr_;
	destruction_queue_manager dqm_;
	transfer_manager tmgr_;
	graphics_manager gmgr_;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_t_;
	std::shared_ptr<const framebuffer_config> fbcfg_;
	std::shared_ptr<const pipeline_layout> pll_;
	std::shared_ptr<const subpass_graph> spg_;

public:
	graphics_test();
	~graphics_test();
	void run();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_TEST_HPP_ */
