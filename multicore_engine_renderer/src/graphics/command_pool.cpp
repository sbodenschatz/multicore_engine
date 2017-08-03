/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/graphics/command_pool.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/command_pool.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

command_pool::command_pool(device& dev, uint32_t queue_family_index, bool transient, bool buffer_resettable)
		: owner_device_(dev) {
	vk::CommandPoolCreateInfo ci;
	ci.queueFamilyIndex = queue_family_index;
	if(transient) ci.flags = ci.flags | vk::CommandPoolCreateFlagBits::eTransient;
	if(buffer_resettable) ci.flags = ci.flags | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	native_command_pool_ = dev->createCommandPoolUnique(ci);
}

void command_pool::reset(bool release_resources) {
	if(release_resources) {
		owner_device_->resetCommandPool(native_command_pool_.get(),
										vk::CommandPoolResetFlagBits::eReleaseResources);
	} else {
		owner_device_->resetCommandPool(native_command_pool_.get(), vk::CommandPoolResetFlags());
	}
}

vk::UniqueCommandBuffer command_pool::allocate_primary_command_buffer() {
	vk::CommandBufferAllocateInfo ai(native_command_pool_.get(), vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer buf;
	vk::Result res = owner_device_->allocateCommandBuffers(&ai, &buf);
	vk::CommandBufferDeleter del(owner_device_.native_device(), native_command_pool_.get());
	return vk::UniqueCommandBuffer(vk::createResultValue(res, buf, "vk::Device::allocateCommandBuffers"),
								   del);
}
vk::UniqueCommandBuffer command_pool::allocate_secondary_command_buffer() {
	vk::CommandBufferAllocateInfo ai(native_command_pool_.get(), vk::CommandBufferLevel::eSecondary, 1);
	vk::CommandBuffer buf;
	vk::Result res = owner_device_->allocateCommandBuffers(&ai, &buf);
	vk::CommandBufferDeleter del(owner_device_.native_device(), native_command_pool_.get());
	return vk::UniqueCommandBuffer(vk::createResultValue(res, buf, "vk::Device::allocateCommandBuffers"),
								   del);
}

std::vector<vk::UniqueCommandBuffer> command_pool::allocate_primary_command_buffers(uint32_t buffer_count) {
	return owner_device_->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(
			native_command_pool_.get(), vk::CommandBufferLevel::ePrimary, buffer_count));
}
std::vector<vk::UniqueCommandBuffer> command_pool::allocate_secondary_command_buffers(uint32_t buffer_count) {
	return owner_device_->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(
			native_command_pool_.get(), vk::CommandBufferLevel::eSecondary, buffer_count));
}

} /* namespace graphics */
} /* namespace mce */
