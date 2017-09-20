/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/windowing/window_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/glfw/monitor.hpp>
#include <mce/glfw/window.hpp>
#include <mce/util/string_tools.hpp>
#include <mce/windowing/window_system.hpp>

namespace mce {
namespace windowing {

window_system::window_system(core::engine& eng, const std::string& window_title)
		: eng{eng}, window_title_{std::move(window_title)}, instance_(), window_() {
	auto mode = eng.config_store().resolve<std::string>("window.display_mode", "windowed");
	auto mode_str = mode->value();
	if(util::equal_ignore_case(mode_str, "windowed_fullscreen")) {
		auto monitor = eng.config_store().resolve("window.monitor", 0);
		auto monitors = glfw::monitor::monitors(instance_);
		auto monitor_index = std::min<size_t>(monitor->value(), monitors.size() - 1);
		window_ = std::make_unique<glfw::window>(window_title_, monitors.at(monitor_index));
	} else if(util::equal_ignore_case(mode_str, "fullscreen")) {
		auto monitor = eng.config_store().resolve("window.monitor", 0);
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
		auto video_mode = eng.config_store().resolve<int>("window.video_mode", int(it - video_modes.begin()));
		auto video_mode_index = size_t(video_mode->value());
		if(video_mode_index >= video_modes.size()) video_mode_index = it - video_modes.begin();
		window_ = std::make_unique<glfw::window>(window_title_, selected_monitor,
												 video_modes.at(video_mode_index));
	} else {
		auto res = eng.config_store().resolve<glm::ivec2>("window.resolution", {800, 600});
		window_ = std::make_unique<glfw::window>(window_title_, res->value());
	}
}

window_system::~window_system() {}

void window_system::preprocess(const mce::core::frame_time&) {
	instance_.poll_events();
	if(window_->should_close()) {
		eng.stop();
	}
}

void window_system::prerender(const mce::core::frame_time&) {
	constexpr size_t fps_update_freq = 60;
	if(++render_frame_counter > fps_update_freq) {
		render_frame_counter = 0;
		auto ft = render_frame_clock.frame_tick();
		window_->title(window_title_ + " (" + std::to_string(fps_update_freq / ft.delta_t) + " fps)");
	}
}

} /* namespace windowing */
} /* namespace mce */
