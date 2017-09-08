/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/input/input_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/core_defs.hpp>
#include <mce/glfw/window.hpp>
#include <mce/input/input_system.hpp>
#include <mce/windowing/window_system.hpp>

namespace mce {
namespace input {

input_system::input_system(core::engine& eng, windowing::window_system& win_sys)
		: eng{eng}, win_sys{win_sys} {}

input_system::~input_system() {}

void input_system::preprocess(const mce::core::frame_time& ft) {
	for(auto& k : key_state_) {
		k.second = win_sys.window().key(k.first);
	}
	last_mouse_state_ = current_mouse_state_;
	current_mouse_state_.position = win_sys.window().cursor_position();
	current_mouse_state_.velocity =
			(current_mouse_state_.position - last_mouse_state_.position) / double(ft.delta_t);
	current_mouse_state_.acceleration =
			(current_mouse_state_.velocity - last_mouse_state_.velocity) / double(ft.delta_t);
	for(auto& mb : {glfw::mouse_button::button_1, glfw::mouse_button::button_2, glfw::mouse_button::button_3,
					glfw::mouse_button::button_4, glfw::mouse_button::button_5, glfw::mouse_button::button_6,
					glfw::mouse_button::button_7, glfw::mouse_button::button_8}) {
		current_mouse_state_.buttons[size_t(mb)] = win_sys.window().mouse_button(mb);
	}
}

} /* namespace input */
} /* namespace mce */
