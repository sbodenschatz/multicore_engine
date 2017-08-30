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

graphics_system::graphics_system(core::engine& eng, core::window_system& win_sys)
		: eng{eng}, device_(instance_), window_(instance_, win_sys.window(), device_),
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
				  })) {}

graphics_system::~graphics_system() {}

} /* namespace graphics */
} /* namespace mce */
