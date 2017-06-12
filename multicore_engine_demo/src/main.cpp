/*
 * Multi-Core Engine project
 * File /mutlicore_engine_demo/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/core/engine.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/application_instance.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/image.hpp>
#include <mce/graphics/window.hpp>

int main() {
	mce::graphics::application_instance ai;
	mce::graphics::device dev(ai);
	mce::glfw::window w("Test", {800, 600});
	mce::graphics::window win(ai, w, dev);
	mce::graphics::device_memory_manager mm(&dev, 1u << 26);
}
