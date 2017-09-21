/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/input/input_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/containers/smart_object_pool_range.hpp>
#include <mce/core/core_defs.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/glfw/window.hpp>
#include <mce/input/input_state.hpp>
#include <mce/input/input_system.hpp>
#include <mce/windowing/window_system.hpp>
#include <tbb/parallel_for.h>

namespace mce {
namespace input {

input_state::input_state(input_system* system, core::game_state*) : system_state(system) {
	system->win_sys.window().cursor_mode(glfw::cursor_mode::disabled);
}

void input_state::reenter(const boost::any&) {
	static_cast<input_system*>(system_)->win_sys.window().cursor_mode(glfw::cursor_mode::disabled);
}

input_state::~input_state() {}

void input_state::register_to_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, first_person_flyer,
								   this->create_first_person_flyer_component(owner, config), this);
}

void input_state::process(const mce::core::frame_time& frame_time) {
	first_person_flyer_comps.process_pending();
	if(frame_time.delta_t > 0.0f) {
		const input_system& sys = *static_cast<input_system*>(system_);
		auto range = containers::make_pool_range(first_person_flyer_comps);
		using range_t = decltype(range);
		tbb::parallel_for(range, [&frame_time, &sys](const range_t& range) {
			for(auto& comp : range) {
				comp.process(frame_time, sys);
			}
		});
	}
}

} /* namespace input */
} /* namespace mce */
