/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/window_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/engine.hpp>
#include <mce/core/window_system.hpp>
#include <mce/glfw/window.hpp>

namespace mce {
namespace core {

window_system::window_system(engine& eng, const std::string& window_title)
		: eng{eng}, instance_(), window_() {
	// TODO Parameterize
	//cppcheck-suppress useInitializationList
	window_ = std::make_unique<glfw::window>(window_title, glm::ivec2{800, 600});
}

window_system::~window_system() {}

void window_system::preprocess(const mce::core::frame_time&) {
	instance_.poll_events();
	if(window_->should_close()) {
		eng.stop();
	}
}

} /* namespace core */
} /* namespace mce */
