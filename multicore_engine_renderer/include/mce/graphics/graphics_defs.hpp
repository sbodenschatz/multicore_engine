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

#include <mce/util/local_function.hpp>
#include <memory>
#include <utility>

namespace mce {
namespace graphics {
class texture;

/// \brief Type used to represent the index of a queue of a vulkan device (consisting of the family index and
/// the index with the family).
typedef std::pair<uint32_t, uint32_t> queue_index_t;
/// Type used to represent the index of a queue family of a vulkan device.
typedef uint32_t queue_family_index_t;

/// Specifies the smart pointer type managing the lifetime of texture objects.
using texture_ptr = std::shared_ptr<const texture>;
/// Specifies the function wrapper type used to wrap completion handlers for texture loading.
using texture_completion_handler = util::local_function<128, void(const texture_ptr& tex)>;

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_GRAPHICS_DEFS_HPP_ */
