/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/window_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/window_system.hpp>
#include <mce/glfw/window.hpp>

namespace mce {
namespace core {

window_system::window_system(engine& eng) : eng{eng}, instance_(), window_() {
	// TODO Parameterize
	window_ = std::make_unique<glfw::window>("multicore_engine", glm::ivec2{800, 600});
}

window_system::~window_system() {}

} /* namespace core */
} /* namespace mce */
