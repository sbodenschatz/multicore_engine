/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/sync_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SYNC_UTILS_HPP_
#define MCE_GRAPHICS_SYNC_UTILS_HPP_

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

vk::AccessFlags required_flags_for_layout(vk::ImageLayout layout);
vk::AccessFlags optional_flags_for_layout(vk::ImageLayout layout);
vk::AccessFlags allowed_flags_for_layout(vk::ImageLayout layout);

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SYNC_UTILS_HPP_ */
