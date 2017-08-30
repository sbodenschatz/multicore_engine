/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/window_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/core/window_system.hpp>
#include <mce/glfw/window.hpp>

namespace mce {
namespace core {

window_system::window_system(engine& eng, const std::string& window_title)
		: eng{eng}, instance_(), window_() {
	// TODO Parameterize
	auto res = eng.config_store().resolve<glm::ivec2>("resolution", {800, 600});
	//cppcheck-suppress useInitializationList
	window_ = std::make_unique<glfw::window>(window_title, res->value());
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
