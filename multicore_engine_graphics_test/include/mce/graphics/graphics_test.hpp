/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_TEST_HPP_
#define MCE_GRAPHICS_GRAPHICS_TEST_HPP_

#include <mce/glfw/instance.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/instance.hpp>

namespace mce {
namespace graphics {

class graphics_test {
	glfw::instance glfw_inst_;
	glfw::window glfw_win_;

public:
	graphics_test();
	~graphics_test();
	void run();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_TEST_HPP_ */
