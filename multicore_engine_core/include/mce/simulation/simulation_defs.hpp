/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/simulation_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_SIMULATION_DEFS_HPP_
#define MCE_SIMULATION_SIMULATION_DEFS_HPP_

#include <mce/util/local_function.hpp>

namespace mce {
namespace core {
struct frame_time;
} // namespace core
namespace entity {
class entity;
} // namespace entity
namespace simulation {

using movement_pattern_function =
		util::local_function<256, void(const mce::core::frame_time&, entity::entity&)>;

} // namespace simulation
} // namespace mce

#endif /* MCE_SIMULATION_SIMULATION_DEFS_HPP_ */
