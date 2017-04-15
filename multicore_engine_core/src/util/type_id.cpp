/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/type_id.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <util/type_id.hpp>

namespace mce {
namespace util {

std::atomic<type_id::type_id_t> type_id::next_id{1};

} // namespace util
} // namespace mce
