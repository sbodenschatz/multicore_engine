/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/simulation/actuator_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/simulation/actuator_system.hpp>

namespace mce {
namespace simulation {

actuator_system::actuator_system(core::engine& eng) : eng_{eng} {}

actuator_system::~actuator_system() {}

} /* namespace simulation */
} /* namespace mce */
