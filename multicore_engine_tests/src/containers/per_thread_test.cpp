/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/per_thread_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/containers/per_thread.hpp>

namespace mce {
namespace containers {

static per_thread<int> per_thread_test_instance{16, 42};

} // namespace containers
} // namespace mce
