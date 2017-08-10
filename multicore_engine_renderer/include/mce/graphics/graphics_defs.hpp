/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/graphics_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_GRAPHICS_DEFS_HPP_
#define GRAPHICS_GRAPHICS_DEFS_HPP_

/**
 * \file
 * Provides general definitions for the graphics subsystem.
 */

#include <boost/optional.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// \brief Type used to represent the index of a queue of a vulkan device (consisting of the family index and
/// the index with the family).
typedef std::pair<uint32_t, uint32_t> queue_index_t;
/// Type used to represent the index of a queue family of a vulkan device.
typedef uint32_t queue_family_index_t;

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_GRAPHICS_DEFS_HPP_ */
