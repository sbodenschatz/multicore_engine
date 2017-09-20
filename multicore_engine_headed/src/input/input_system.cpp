/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/input/input_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/config/config_store.hpp>
#include <mce/core/core_defs.hpp>
#include <mce/core/engine.hpp>
#include <mce/glfw/name_mapping.hpp>
#include <mce/glfw/window.hpp>
#include <mce/input/input_system.hpp>
#include <mce/util/string_tools.hpp>
#include <mce/windowing/window_system.hpp>

namespace mce {
namespace input {

input_system::input_system(core::engine& eng, windowing::window_system& win_sys)
		: eng{eng}, win_sys{win_sys}, var_mouse_sensitivity_percent{eng.config_store().resolve(
											  "input.mouse_sensitivity_percent", 100.0f)} {
	for(auto k : glfw::all_keys()) {
		current_key_state_[k] = false;
		last_key_state_[k] = false;
	}
	current_mouse_state_.position = win_sys.window().cursor_position();
	current_mouse_state_.velocity = {};
	current_mouse_state_.acceleration = {};
	current_mouse_state_.raw_velocity = {};
	current_mouse_state_.raw_acceleration = {};
	last_mouse_state_ = current_mouse_state_;
}

input_system::~input_system() {}

void input_system::preprocess(const mce::core::frame_time& ft) {
	using std::swap;
	swap(current_key_state_, last_key_state_);
	for(auto& k : current_key_state_) {
		k.second = win_sys.window().key(k.first);
	}
	last_mouse_state_ = current_mouse_state_;
	current_mouse_state_.position = win_sys.window().cursor_position();
	current_mouse_state_.velocity = double(var_mouse_sensitivity_percent->value() / 100.0f) *
									(current_mouse_state_.position - last_mouse_state_.position) /
									double(ft.delta_t);
	current_mouse_state_.acceleration =
			(current_mouse_state_.velocity - last_mouse_state_.velocity) / double(ft.delta_t);
	current_mouse_state_.raw_velocity =
			(current_mouse_state_.position - last_mouse_state_.position) / double(ft.delta_t);
	current_mouse_state_.raw_acceleration =
			(current_mouse_state_.raw_velocity - last_mouse_state_.raw_velocity) / double(ft.delta_t);
	for(auto& mb : {glfw::mouse_button::button_1, glfw::mouse_button::button_2, glfw::mouse_button::button_3,
					glfw::mouse_button::button_4, glfw::mouse_button::button_5, glfw::mouse_button::button_6,
					glfw::mouse_button::button_7, glfw::mouse_button::button_8}) {
		current_mouse_state_.buttons[size_t(mb)] = win_sys.window().mouse_button(mb);
	}
}
glfw::key input_system::key_from_name_or_config(const std::string& name) const {
	{
		std::shared_lock<std::shared_timed_mutex> rlock(mtx);
		auto it = key_name_cache.find(name);
		if(it != key_name_cache.end()) {
			return it->second;
		}
	}
	using namespace std::literals;
	{
		std::unique_lock<std::shared_timed_mutex> rwlock(mtx);
		auto it = key_name_cache.find(name);
		if(it != key_name_cache.end()) {
			return it->second;
		} else if(util::starts_with(name, "%"s)) {
			auto pos = name.find('%', 1);
			if(pos == name.npos) {
				auto var = eng.config_store().resolve("input.key."s + name.substr(1), "unknown"s);
				auto key = glfw::key_from_string(var->value());
				key_name_cache.emplace(name, key);
				return key;
			} else {
				auto var = eng.config_store().resolve("input.key."s + name.substr(1, pos - 1),
													  name.substr(pos + 1));
				auto key = glfw::key_from_string(var->value());
				key_name_cache.emplace(name, key);
				return key;
			}
		} else {
			auto key = glfw::key_from_string(name);
			key_name_cache.emplace(name, key);
			return key;
		}
	}
}

} /* namespace input */
} /* namespace mce */
