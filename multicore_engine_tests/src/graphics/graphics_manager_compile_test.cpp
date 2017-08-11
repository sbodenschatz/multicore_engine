/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/graphics_manager_compile_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/graphics_manager.hpp>

#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/instance.hpp>

#include <gtest.hpp>

#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

static void test_graphics_manager_compilation() {
	mce::graphics::instance ai;
	mce::graphics::device dev(ai);
	mce::graphics::destruction_queue_manager dqm(&dev, 3);
	mce::graphics::graphics_manager gm(dev, &dqm);
}

TEST(graphics_graphics_manager, compile_test) {
	UNUSED(&test_graphics_manager_compilation);
	ASSERT_TRUE(true);
}
}
}
