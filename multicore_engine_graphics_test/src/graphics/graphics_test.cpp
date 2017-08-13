/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <glm/vec2.hpp>
#include <mce/graphics/graphics_test.hpp>

namespace mce {
namespace graphics {

graphics_test::graphics_test() : glfw_win_("Vulkan Test", glm::ivec2(800, 600)), dev_(inst_) {}

graphics_test::~graphics_test() {}

void graphics_test::run() {
	while(!glfw_win_.should_close()) {
		glfw_inst_.poll_events();
	}
}

} /* namespace graphics */
} /* namespace mce */
