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

/// Records a queue ownership transfer on the given buffer into the given command buffers for both queues.
void buffer_queue_ownership_transfer(vk::Buffer buffer, vk::CommandBuffer cb_queue_src,
									 vk::CommandBuffer cb_queue_dst, uint32_t queue_family_src,
									 uint32_t queue_family_dst, vk::PipelineStageFlags stage_mask_src,
									 vk::PipelineStageFlags stage_mask_dst, vk::AccessFlags access_flags_src,
									 vk::AccessFlags access_flags_dst);

void image_queue_ownership_transfer(vk::Image image, vk::ImageLayout layout, vk::CommandBuffer cb_queue_src,
									vk::CommandBuffer cb_queue_dst, uint32_t queue_family_src,
									uint32_t queue_family_dst, vk::PipelineStageFlags stage_mask_src,
									vk::PipelineStageFlags stage_mask_dst, vk::AccessFlags access_flags_src,
									vk::AccessFlags access_flags_dst,
									vk::ImageAspectFlags aspects = vk::ImageAspectFlagBits::eColor);

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SYNC_UTILS_HPP_ */
