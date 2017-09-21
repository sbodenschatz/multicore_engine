/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/containers/smart_object_pool_range.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/simulation/actuator_state.hpp>
#include <tbb/parallel_for.h>

namespace mce {
namespace simulation {

actuator_state::actuator_state(core::system* system, core::game_state*) : system_state(system) {}

actuator_state::~actuator_state() {}

void actuator_state::register_to_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, actuator, this->create_actuator_component(owner, config), this);
}

void actuator_state::process(const mce::core::frame_time& frame_time) {
	actuator_comps.process_pending();
	tbb::parallel_for(
			containers::make_pool_range(actuator_comps),
			[&frame_time](containers::smart_object_pool_range<decltype(actuator_comps)::iterator>& range) {
				for(auto& ac : range) {
					ac.process(frame_time);
				}
			});
}

} /* namespace simulation */
} /* namespace mce */
