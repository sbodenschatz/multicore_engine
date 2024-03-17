/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cctype>
#include <mce/config/config_store.hpp>
#include <mce/core/engine.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/graphics/sync_utils.hpp>
#include <mce/windowing/window_system.hpp>

namespace mce {
namespace graphics {

graphics_system::graphics_system(core::engine& eng, windowing::window_system& win_sys,
								 const std::vector<std::string>& extensions, unsigned int validation_level)
		: eng{eng},
		  instance_(eng.engine_metadata(), eng.application_metadata(), extensions, validation_level),
		  device_(instance_, device_type_prefs_from_config(), device_prefs_from_config()),
		  window_(instance_, win_sys.window(), device_, desired_swapchain_images_from_config(),
				  present_mode_prefs_from_config()),
		  memory_manager_(&device_, memory_block_size_from_config()),
		  destruction_queue_manager_(&device_, uint32_t(window_.swapchain_images().size())),
		  transfer_manager_(device_, memory_manager_, uint32_t(window_.swapchain_images().size())),
		  texture_manager_(eng.asset_manager(), device_, memory_manager_, &destruction_queue_manager_,
						   transfer_manager_),
		  graphics_manager_(device_, &destruction_queue_manager_),
		  tmp_semaphore_(device_->createSemaphoreUnique({})),
		  acquire_semaphores_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return device_->createSemaphoreUnique({});
							  })),
		  pre_present_semaphores_(
				  window_.swapchain_images().size(),
				  containers::generator_param([this](size_t) { return device_->createSemaphoreUnique({}); })),
		  present_semaphores_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
								  return device_->createSemaphoreUnique({});
							  })),
		  fences_(window_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return device_->createFenceUnique(
							  vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
				  })),
		  current_swapchain_image_{0}, render_queue_cmd_pool_(device_, device_.graphics_queue_index().first),
		  present_queue_cmd_pool_(device_, device_.present_queue_index().first),
		  render_queue_start_frame_cmd_buffers_(
				  window_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return render_queue_cmd_pool_.allocate_primary_command_buffer();
				  })),
		  render_queue_end_frame_cmd_buffers_(
				  window_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return render_queue_cmd_pool_.allocate_primary_command_buffer();
				  })),
		  present_queue_end_frame_cmd_buffers_(
				  window_.swapchain_images().size(), containers::generator_param([this](size_t) {
					  return present_queue_cmd_pool_.allocate_primary_command_buffer();
				  })) {
	for(uint32_t i = 0; i < window_.swapchain_images().size(); ++i) {
		render_queue_start_frame_cmd_buffers_[i]->begin(vk::CommandBufferBeginInfo({}, {}));
		render_queue_end_frame_cmd_buffers_[i]->begin(vk::CommandBufferBeginInfo({}, {}));
		present_queue_end_frame_cmd_buffers_[i]->begin(
				vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse, {}));
		// TODO Minimize barriers.
		render_queue_start_frame_cmd_buffers_[i]->pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
				{base_image::generate_transition_native(
						window_.swapchain_images()[i], vk::ImageLayout::eUndefined,
						vk::ImageLayout::eColorAttachmentOptimal, {},
						allowed_flags_for_layout(vk::ImageLayout::eColorAttachmentOptimal),
						vk::ImageAspectFlagBits::eColor)});
		render_queue_end_frame_cmd_buffers_[i]->pipelineBarrier(
				vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
				{base_image::generate_transition_native(
						window_.swapchain_images()[i], vk::ImageLayout::eColorAttachmentOptimal,
						vk::ImageLayout::ePresentSrcKHR,
						allowed_flags_for_layout(vk::ImageLayout::eColorAttachmentOptimal),
						allowed_flags_for_layout(vk::ImageLayout::ePresentSrcKHR),
						vk::ImageAspectFlagBits::eColor)});
		if(device_.graphics_queue_index().first != device_.present_queue_index().first) {
			image_queue_ownership_transfer(
					window_.swapchain_images()[i], vk::ImageLayout::ePresentSrcKHR,
					render_queue_end_frame_cmd_buffers_[i].get(),
					present_queue_end_frame_cmd_buffers_[i].get(), device_.graphics_queue_index().first,
					device_.present_queue_index().first, vk::PipelineStageFlagBits::eAllCommands,
					vk::PipelineStageFlagBits::eAllCommands,
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
					~vk::AccessFlags{}, vk::ImageAspectFlagBits::eColor);
		}
		render_queue_start_frame_cmd_buffers_[i]->end();
		render_queue_end_frame_cmd_buffers_[i]->end();
		present_queue_end_frame_cmd_buffers_[i]->end();
	}
}

graphics_system::~graphics_system() {
	device_->waitIdle();
}

void graphics_system::prerender(const mce::core::frame_time&) {
	auto acq_res =
			device_->acquireNextImageKHR(window_.swapchain(), ~0ull, tmp_semaphore_.get(), vk::Fence());
	if(acq_res.result == vk::Result::eSuccess) {
		current_swapchain_image_ = acq_res.value;
		auto fence_wait_res = device_->waitForFences(fences_[current_swapchain_image_].get(), true, ~0u);
		if(fence_wait_res == vk::Result::eTimeout) {
			throw mce::graphics_exception("waiting for fence timed out.");
		}
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
	cmd_buff_handles_.clear();
	cmd_buff_handles_.push_back(render_queue_start_frame_cmd_buffers_[current_swapchain_image_].get());
	struct handle_transformer : boost::static_visitor<vk::CommandBuffer> {
		vk::CommandBuffer operator()(vk::CommandBuffer cb) const {
			return cb;
		}
		vk::CommandBuffer operator()(const queued_handle<vk::UniqueCommandBuffer>& cb) const {
			return cb.get();
		}
	} ht;
	std::transform(pending_command_buffers_.begin(), pending_command_buffers_.end(),
				   std::back_inserter(cmd_buff_handles_),
				   [&ht](const auto& h) { return h.apply_visitor(ht); });
	cmd_buff_handles_.push_back(render_queue_end_frame_cmd_buffers_[current_swapchain_image_].get());
	auto acq_sema = acquire_semaphores_[current_swapchain_image_].get();
	vk::PipelineStageFlags wait_ps = vk::PipelineStageFlagBits::eAllCommands;
	auto pre_present_sema = pre_present_semaphores_[current_swapchain_image_].get();
	auto present_sema = present_semaphores_[current_swapchain_image_].get();
	auto ownership_transfer_needed =
			device_.graphics_queue_index().first != device_.present_queue_index().first;
	device_.graphics_queue().submit(
			{vk::SubmitInfo(1, &acq_sema, &wait_ps, uint32_t(cmd_buff_handles_.size()),
							cmd_buff_handles_.data(), 1,
							ownership_transfer_needed ? &pre_present_sema : &present_sema)},
			fences_[current_swapchain_image_].get());
	if(ownership_transfer_needed) {
		auto present_buff_handle = present_queue_end_frame_cmd_buffers_[current_swapchain_image_].get();
		device_.present_queue().submit(
				{vk::SubmitInfo(1, &pre_present_sema, &wait_ps, 1, &present_buff_handle, 1, &present_sema)},
				vk::Fence{});
	}
	auto swapchain_handle = window_.swapchain();
	auto present_result = device_.present_queue().presentKHR(
			vk::PresentInfoKHR(1, &present_sema, 1, &swapchain_handle, &current_swapchain_image_));
	if(present_result == vk::Result::eSuboptimalKHR) {
		// TODO: Handle suboptimal
	}
	transfer_manager_.end_frame();
}

static boost::container::flat_map<std::string, vk::PhysicalDeviceType> device_type_from_string_table = {
		{{"other", vk::PhysicalDeviceType::eOther},
		 {"integrated", vk::PhysicalDeviceType::eIntegratedGpu},
		 {"discrete", vk::PhysicalDeviceType::eDiscreteGpu},
		 {"virtual", vk::PhysicalDeviceType::eVirtualGpu},
		 {"cpu", vk::PhysicalDeviceType::eCpu}}};

vk::PhysicalDeviceType graphics_system::device_type_from_string(const std::string& type) {
	auto it = device_type_from_string_table.find(type);
	if(it != device_type_from_string_table.end()) {
		return it->second;
	}
	throw mce::syntax_exception("Invalid device type '" + type + "'.");
}
std::vector<vk::PhysicalDeviceType> graphics_system::device_type_prefs_from_config() const {
	std::vector<vk::PhysicalDeviceType> device_type_preferences;
	using namespace std::literals;
	std::vector<std::string> device_type_preferences_str = {{{"discrete"s}, {"integrated"s}}};
	auto var_device_type_preferences =
			eng.config_store().resolve("graphics.device_type_preferences"s, device_type_preferences_str);
	device_type_preferences_str = var_device_type_preferences->value();
	for(auto& dts : device_type_preferences_str) {
		for(auto& c : dts) {
			c = char(std::tolower(c));
		}
	}
	std::transform(device_type_preferences_str.begin(), device_type_preferences_str.end(),
				   std::back_inserter(device_type_preferences),
				   [](const std::string& dts) { return device_type_from_string(dts); });
	return device_type_preferences;
}
std::vector<std::string> graphics_system::device_prefs_from_config() const {
	std::vector<std::string> device_preferences;
	return eng.config_store().resolve("graphics.device_preferences", device_preferences)->value();
}
vk::DeviceSize graphics_system::memory_block_size_from_config() const {
	auto var_memory_block_size_exp = eng.config_store().resolve("graphics.memory_block_size_exp", 27);
	return uint64_t(1) << var_memory_block_size_exp->value();
}
uint32_t graphics_system::desired_swapchain_images_from_config() const {
	auto var_desired_swapchain_images = eng.config_store().resolve("graphics.desired_swapchain_images", 3);
	return var_desired_swapchain_images->value();
}
static boost::container::flat_map<std::string, vk::PresentModeKHR> present_mode_from_string_table = {
		{{"fifo", vk::PresentModeKHR::eFifo},
		 {"fifo_relaxed", vk::PresentModeKHR::eFifoRelaxed},
		 {"fiforelaxed", vk::PresentModeKHR::eFifoRelaxed},
		 {"immediate", vk::PresentModeKHR::eImmediate},
		 {"mailbox", vk::PresentModeKHR::eMailbox}}};
vk::PresentModeKHR graphics_system::present_mode_from_string(const std::string& mode) {
	auto it = present_mode_from_string_table.find(mode);
	if(it != present_mode_from_string_table.end()) {
		return it->second;
	}
	throw mce::syntax_exception("Invalid present mode '" + mode + "'.");
}
std::vector<vk::PresentModeKHR> graphics_system::present_mode_prefs_from_config() const {
	std::vector<vk::PresentModeKHR> present_mode_preferences;
	using namespace std::literals;
	std::vector<std::string> present_mode_preferences_str = {
			{{"mailbox"s}, {"fifo_relaxed"s}, {"fifo"s}, {"immediate"s}}};
	auto var_present_mode_preferences =
			eng.config_store().resolve("graphics.present_mode_preferences"s, present_mode_preferences_str);
	present_mode_preferences_str = var_present_mode_preferences->value();
	for(auto& dts : present_mode_preferences_str) {
		for(auto& c : dts) {
			c = char(std::tolower(c));
		}
	}
	std::transform(present_mode_preferences_str.begin(), present_mode_preferences_str.end(),
				   std::back_inserter(present_mode_preferences),
				   [](const std::string& dts) { return present_mode_from_string(dts); });
	return present_mode_preferences;
}

} /* namespace graphics */
} /* namespace mce */
