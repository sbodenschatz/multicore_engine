/*
 * Multi-Core Engine project
 * File /mutlicore_engine_graphics_test/src/main.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/graphics_test.hpp>

int main(int, char* argv[]) {
	mce::graphics::graphics_test::exe_path = boost::filesystem::path(argv[0]).parent_path();
	mce::graphics::graphics_test gt;
	gt.run();
}
