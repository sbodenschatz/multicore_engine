/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/unused.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_UNUSED_HPP_
#define UTIL_UNUSED_HPP_

/// Marks an expression as intendedly unused to silence compiler warnings about unused variables.
#define UNUSED(X) static_cast<void>(X)

#endif /* UTIL_UNUSED_HPP_ */
