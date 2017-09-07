/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/sync_utils.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/sync_utils.hpp>

namespace mce {
namespace graphics {

// TODO Cite in sources:
// https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/blob/57f0da98098689f3624c5503cfe1a4b5fede885a/layers/buffer_validation.cpp#L2691

vk::AccessFlags required_flags_for_layout(vk::ImageLayout layout) {
	switch(layout) {
	case vk::ImageLayout::eColorAttachmentOptimal: return vk::AccessFlagBits::eColorAttachmentWrite;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	case vk::ImageLayout::eTransferDstOptimal: return vk::AccessFlagBits::eTransferWrite;
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal: return vk::AccessFlags{};
	case vk::ImageLayout::eShaderReadOnlyOptimal: return vk::AccessFlags{};
	case vk::ImageLayout::eTransferSrcOptimal: return vk::AccessFlagBits::eTransferRead;
	case vk::ImageLayout::eUndefined: return vk::AccessFlags{};
	case vk::ImageLayout::ePresentSrcKHR: return vk::AccessFlagBits::eMemoryRead;
	default: return vk::AccessFlags{};
	}
}
vk::AccessFlags optional_flags_for_layout(vk::ImageLayout layout) {
	switch(layout) {
	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eInputAttachmentRead;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eInputAttachmentRead;
	case vk::ImageLayout::eTransferDstOptimal: return vk::AccessFlags{};
	case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eInputAttachmentRead |
			   vk::AccessFlagBits::eShaderRead;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::AccessFlagBits::eInputAttachmentRead | vk::AccessFlagBits::eShaderRead;
	case vk::ImageLayout::eTransferSrcOptimal: return vk::AccessFlags{};
	case vk::ImageLayout::eUndefined: return vk::AccessFlags{};
	case vk::ImageLayout::ePresentSrcKHR: return vk::AccessFlags{};
	default: return ~vk::AccessFlags{};
	}
}
vk::AccessFlags allowed_flags_for_layout(vk::ImageLayout layout) {
	return required_flags_for_layout(layout) | optional_flags_for_layout(layout);
}

void buffer_queue_ownership_transfer(vk::Buffer buffer, vk::CommandBuffer cb_queue_src,
									 vk::CommandBuffer cb_queue_dst, uint32_t queue_family_src,
									 uint32_t queue_family_dst, vk::PipelineStageFlags stage_mask_src,
									 vk::PipelineStageFlags stage_mask_dst, vk::AccessFlags access_flags_src,
									 vk::AccessFlags access_flags_dst) {
	cb_queue_src.pipelineBarrier(stage_mask_src, stage_mask_dst, {}, {},
								 {vk::BufferMemoryBarrier(access_flags_src, {}, queue_family_src,
														  queue_family_dst, buffer, 0, VK_WHOLE_SIZE)},
								 {});
	cb_queue_dst.pipelineBarrier(stage_mask_src, stage_mask_dst, {}, {},
								 {vk::BufferMemoryBarrier({}, access_flags_dst, queue_family_src,
														  queue_family_dst, buffer, 0, VK_WHOLE_SIZE)},
								 {});
}

void image_queue_ownership_transfer(vk::Image image, vk::ImageLayout layout, vk::CommandBuffer cb_queue_src,
									vk::CommandBuffer cb_queue_dst, uint32_t queue_family_src,
									uint32_t queue_family_dst, vk::PipelineStageFlags stage_mask_src,
									vk::PipelineStageFlags stage_mask_dst, vk::AccessFlags access_flags_src,
									vk::AccessFlags access_flags_dst, vk::ImageAspectFlags aspects) {
	cb_queue_src.pipelineBarrier(
			stage_mask_src, stage_mask_dst, {}, {}, {},
			{vk::ImageMemoryBarrier(access_flags_src, {}, layout, layout, queue_family_src, queue_family_dst,
									image, vk::ImageSubresourceRange(aspects, 0, VK_REMAINING_MIP_LEVELS, 0,
																	 VK_REMAINING_ARRAY_LAYERS))});
	cb_queue_dst.pipelineBarrier(
			stage_mask_src, stage_mask_dst, {}, {}, {},
			{vk::ImageMemoryBarrier({}, access_flags_dst, layout, layout, queue_family_src, queue_family_dst,
									image, vk::ImageSubresourceRange(aspects, 0, VK_REMAINING_MIP_LEVELS, 0,
																	 VK_REMAINING_ARRAY_LAYERS))});
}

} /* namespace graphics */
} /* namespace mce */
