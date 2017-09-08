/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/system_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/system_state.hpp>

namespace mce {
namespace core {
void system_state::process(const mce::core::frame_time&) {}
void system_state::render(const mce::core::frame_time&) {}
void system_state::leave_pop() {}
void system_state::leave_push() {}
void system_state::reenter(const boost::any&) {}

} /* namespace core */
} /* namespace mce */
