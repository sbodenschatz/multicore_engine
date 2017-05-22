/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/system.hpp>

namespace mce {
namespace core {
void system::preprocess(const mce::core::frame_time&) {}
void system::postprocess(const mce::core::frame_time&) {}
void system::prerender(const mce::core::frame_time&) {}
void system::postrender(const mce::core::frame_time&) {}
} /* namespace core */
} /* namespace mce */
