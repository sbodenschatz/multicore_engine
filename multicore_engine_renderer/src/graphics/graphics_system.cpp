/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/engine.hpp>
#include <mce/core/window_system.hpp>
#include <mce/graphics/graphics_system.hpp>

namespace mce {
namespace graphics {

graphics_system::graphics_system(core::engine& eng, core::window_system& win_sys,
								 const std::vector<std::string>& extensions, unsigned int validation_level)
		: eng{eng},
		  instance_(eng.engine_metadata(), eng.application_metadata(), extensions, validation_level),
		  device_(instance_), window_(instance_, win_sys.window(), device_),
		  // TODO Parameterize
		  memory_manager_(&device_, 1 << 27),
		  destruction_queue_manager_(&device_, uint32_t(window_.swapchain_images().size())),
		  transfer_manager_(device_, memory_manager_, uint32_t(window_.swapchain_images().size())),
		  texture_manager_(eng.asset_manager(), device_, memory_manager_, &destruction_queue_manager_,
						   transfer_manager_),
		  graphics_manager_(device_, &destruction_queue_manager_),
		  tmp_semaphore_(device_->createSemaphoreUnique({})),
		  acquire_semaphores_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return device_->createSemaphoreUnique({});
							  })),
		  present_semaphores_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return device_->createSemaphoreUnique({});
							  })),
		  fences_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return device_->createFenceUnique(
							  vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
				  })),
		  current_swapchain_image_{0} {}

graphics_system::~graphics_system() {}

void graphics_system::prerender(const mce::core::frame_time&) {
	auto acq_res =
			device_->acquireNextImageKHR(window_.swapchain(), ~0ull, tmp_semaphore_.get(), vk::Fence());
	if(acq_res.result == vk::Result::eSuccess) {
		current_swapchain_image_ = acq_res.value;
		device_->waitForFences(fences_[current_swapchain_image_].get(), true, ~0u);
		device_->resetFences(fences_[current_swapchain_image_].get());
		using std::swap;
		swap(tmp_semaphore_, acquire_semaphores_[current_swapchain_image_]);
		transfer_manager_.start_frame(current_swapchain_image_);
		destruction_queue_manager_.cleanup_and_set_current(current_swapchain_image_);
		pending_command_buffers_ = transfer_manager_.retrieve_ready_ownership_transfers();
	} else
		throw mce::graphics_exception("acquire failed.");
	// TODO Handle non-success, non-failure (e.g. suboptimal)
}
void graphics_system::postrender(const mce::core::frame_time&) {
	cmd_buff_handles.clear();
	std::transform(pending_command_buffers_.begin(), pending_command_buffers_.end(),
				   std::back_inserter(cmd_buff_handles), [](const auto& h) { return h.get(); });
	auto acq_sema = acquire_semaphores_[current_swapchain_image_].get();
	vk::PipelineStageFlags wait_ps = vk::PipelineStageFlagBits::eTopOfPipe;
	auto present_sema = present_semaphores_[current_swapchain_image_].get();
	device_.graphics_queue().submit({vk::SubmitInfo(1, &acq_sema, &wait_ps, uint32_t(cmd_buff_handles.size()),
													cmd_buff_handles.data(), 1, &present_sema)},
									fences_[current_swapchain_image_].get());
	auto swapchain_handle = window_.swapchain();
	device_.present_queue().presentKHR(
			vk::PresentInfoKHR(1, &present_sema, 1, &swapchain_handle, &current_swapchain_image_));
	transfer_manager_.end_frame();
}

} /* namespace graphics */
} /* namespace mce */
