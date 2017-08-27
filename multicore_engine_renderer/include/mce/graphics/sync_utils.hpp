/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/sync_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SYNC_UTILS_HPP_
#define MCE_GRAPHICS_SYNC_UTILS_HPP_

/**
 * \file
 * Defines helper functions for synchronization in vulkan.
 * - flag_for_layout functions can be used to calculate access flags for layout transitions based on the
 * layout as checked by validation layers.
 */

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Returns all access flags required for layout transitions from / to the given layout.
vk::AccessFlags required_flags_for_layout(vk::ImageLayout layout);
/// \brief Returns all access flags that can optionally be specified for layout transitions from / to the
/// given layout.
vk::AccessFlags optional_flags_for_layout(vk::ImageLayout layout);
/// Returns all access flags that are valid for layout transitions from / to the given layout.
vk::AccessFlags allowed_flags_for_layout(vk::ImageLayout layout);

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SYNC_UTILS_HPP_ */
