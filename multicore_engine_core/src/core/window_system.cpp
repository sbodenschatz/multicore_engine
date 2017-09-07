/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/window_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/core/window_system.hpp>
#include <mce/glfw/monitor.hpp>
#include <mce/glfw/window.hpp>
#include <mce/util/string_tools.hpp>

namespace mce {
namespace core {

window_system::window_system(engine& eng, const std::string& window_title)
		: eng{eng}, instance_(), window_() {
	auto mode = eng.config_store().resolve<std::string>("display_mode", "windowed");
	auto mode_str = mode->value();
	if(util::equal_ignore_case(mode_str, "windowed_fullscreen")) {
		auto monitor = eng.config_store().resolve("monitor", 0);
		auto monitors = glfw::monitor::monitors(instance_);
		auto monitor_index = std::min<size_t>(monitor->value(), monitors.size() - 1);
		window_ = std::make_unique<glfw::window>(window_title, monitors.at(monitor_index));
	} else if(util::equal_ignore_case(mode_str, "fullscreen")) {
		auto monitor = eng.config_store().resolve("monitor", 0);
		auto monitors = glfw::monitor::monitors(instance_);
		auto monitor_index = std::min<size_t>(monitor->value(), monitors.size() - 1);
		auto& selected_monitor = monitors.at(monitor_index);
		auto video_modes = selected_monitor.video_modes();
		auto cv = selected_monitor.current_video_mode();
		auto it = std::find_if(video_modes.begin(), video_modes.end(), [&cv](const glfw::video_mode& v1) {
			return std::tie(v1.width, v1.height, v1.red_bits, v1.green_bits, v1.blue_bits, v1.refresh_rate) ==
				   std::tie(cv.width, cv.height, cv.red_bits, cv.green_bits, cv.blue_bits, cv.refresh_rate);
		});
		if(it == video_modes.end()) it = video_modes.end() - 1;
		auto video_mode = eng.config_store().resolve<int>("video_mode", int(it - video_modes.begin()));
		auto video_mode_index = size_t(video_mode->value());
		if(video_mode_index >= video_modes.size()) video_mode_index = it - video_modes.begin();
		window_ = std::make_unique<glfw::window>(window_title, selected_monitor,
												 video_modes.at(video_mode_index));
	} else {
		auto res = eng.config_store().resolve<glm::ivec2>("resolution", {800, 600});
		window_ = std::make_unique<glfw::window>(window_title, res->value());
	}
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
