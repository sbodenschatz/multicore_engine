/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/graphics/device.cpp
 * Copyright 2016-2018 by Stefan Bodenschatz
 */

#ifdef MULTICORE_ENGINE_WINDOWS
#include <windows.h> //Fix macro redefinition error in glfw header
#endif

#include <GLFW/glfw3.h>
#include <algorithm>
#include <boost/container/flat_map.hpp>
#include <boost/range/irange.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <mce/exceptions.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/util/algorithm.hpp>
#include <mce/util/unused.hpp>
#include <vector>

#if(!defined(GLM_DEPTH_CLIP_SPACE) || GLM_DEPTH_CLIP_SPACE != GLM_DEPTH_ZERO_TO_ONE) &&                      \
		((GLM_CONFIG_CLIP_CONTROL & GLM_CLIP_CONTROL_ZO_BIT) == 0)
#error "A GLM version supporting GLM_FORCE_DEPTH_ZERO_TO_ONE is required for vulkan."
#endif

namespace mce {
namespace graphics {

const std::pair<uint32_t, uint32_t> device::no_queue_index{~0u, ~0u};
const uint32_t device::no_queue_family_index{~0u};

device::device(instance& app_inst, std::vector<vk::PhysicalDeviceType> device_type_preferences,
			   std::vector<std::string> device_preferences)
		: device_type_preferences_{std::move(device_type_preferences)},
		  device_preferences_{std::move(device_preferences)}, instance_{app_inst} {
	find_physical_device();
	find_queue_indexes();
	create_device();
}

queue_family_index_t
device::find_queue_family(const std::vector<vk::QueueFamilyProperties>& queue_families,
						  vk::QueueFlags required_flags, bool present_required,
						  boost::container::flat_set<queue_family_index_t> excluded_families) {
	for(queue_family_index_t queue_family_index = 0; queue_family_index < queue_families.size();
		++queue_family_index) {
		if(excluded_families.count(queue_family_index) < 1 &&
		   (queue_families[queue_family_index].queueFlags & required_flags) == required_flags &&
		   (!present_required ||
			glfwGetPhysicalDevicePresentationSupport(instance_.native_instance(), physical_device_,
													 queue_family_index))) {
			return queue_family_index;
		}
	}
	return no_queue_family_index;
}

queue_index_t device::find_queue(const std::vector<vk::QueueFamilyProperties>& queue_families,
								 vk::QueueFlags required_flags, bool present_required,
								 boost::container::flat_set<queue_family_index_t> excluded_families,
								 boost::container::flat_set<queue_index_t> excluded_queues) {
	for(queue_family_index_t queue_family_index = 0; queue_family_index < queue_families.size();
		++queue_family_index) {
		if(excluded_families.count(queue_family_index) < 1 &&
		   (queue_families[queue_family_index].queueFlags & required_flags) == required_flags &&
		   (!present_required ||
			glfwGetPhysicalDevicePresentationSupport(instance_.native_instance(), physical_device_,
													 queue_family_index))) {
			for(uint32_t queue_index = 0; queue_index < queue_families[queue_family_index].queueCount;
				++queue_index) {
				if(excluded_queues.count(std::make_pair(queue_family_index, queue_index)) < 1) {
					return std::make_pair(queue_family_index, queue_index);
				}
			}
		}
	}
	return no_queue_index;
}

void device::find_physical_device() {
	std::vector<vk::PhysicalDevice> phy_devs = instance_->enumeratePhysicalDevices();
	std::vector<vk::PhysicalDevice> suitable_phy_devs;
	std::copy_if(
			phy_devs.begin(), phy_devs.end(), std::back_inserter(suitable_phy_devs),
			[this](const vk::PhysicalDevice& pd) {
				auto qfs = boost::irange(0u, uint32_t(pd.getQueueFamilyProperties().size()));
				bool presentation_supported = std::any_of(qfs.begin(), qfs.end(), [this, &pd](uint32_t qf) {
					return glfwGetPhysicalDevicePresentationSupport(instance_.native_instance(), pd, qf);
				});
				auto rfs = required_device_features();
				auto avail_features = pd.getFeatures();
				bool required_features_supported =
						std::all_of(rfs.begin(), rfs.end(),
									[&avail_features](vk::Bool32 vk::PhysicalDeviceFeatures::*rf) {
										return avail_features.*rf;
									});
				auto queue_families = pd.getQueueFamilyProperties();
				bool graphics_supported =
						std::any_of(queue_families.begin(), queue_families.end(),
									[](const vk::QueueFamilyProperties& qfp) {
										return bool(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
									});
				return presentation_supported && required_features_supported && graphics_supported;
			});
	if(suitable_phy_devs.empty()) {
		throw no_suitable_device_found_exception(
				"No vulkan device with required presentation capabilities found.");
	}
	util::preference_sort(suitable_phy_devs, device_type_preferences_,
						  [](const vk::PhysicalDevice& pd) { return pd.getProperties().deviceType; });
	util::preference_sort(suitable_phy_devs, device_preferences_, [](const vk::PhysicalDevice& pd) {
		return std::string(pd.getProperties().deviceName.data());
	});

	physical_device_ = suitable_phy_devs.front();
	physical_device_properties_ = physical_device_.getProperties();
	std::cout << "Using render device \"" << physical_device_properties_.deviceName << "\"" << std::endl;
}

void device::find_queue_indexes() {
	auto queue_families = physical_device_.getQueueFamilyProperties();
	// 1. Try to find distinct queue families for each task (graphics, transfer, present).
	{
		auto graphics_family = find_queue_family(queue_families, vk::QueueFlagBits::eGraphics, false);
		if(graphics_family != no_queue_family_index)
			graphics_queue_index_ = std::make_pair(graphics_family, 0);
		auto transfer_family =
				find_queue_family(queue_families, vk::QueueFlagBits::eTransfer, false, {graphics_family});
		if(transfer_family != no_queue_family_index)
			transfer_queue_index_ = std::make_pair(transfer_family, 0);
		auto present_family =
				find_queue_family(queue_families, vk::QueueFlags(), true, {graphics_family, transfer_family});
		if(present_family != no_queue_family_index) present_queue_index_ = std::make_pair(present_family, 0);
	}
	// 2. Try to find distinct queues for each task where 1. failed.
	if(graphics_queue_index_ == no_queue_index) { // However, if 1. failed for graphics, we don't have a
												  // queue with graphics capability, which is a hard
												  // requirement to render anything.
		throw std::runtime_error("Failed to find a queue family with graphics capability.");
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ =
				find_queue(queue_families, vk::QueueFlagBits::eTransfer, false,
						   boost::container::flat_set<queue_family_index_t>(), {graphics_queue_index_});
	}
	if(present_queue_index_ == no_queue_index) {
		present_queue_index_ =
				find_queue(queue_families, vk::QueueFlags(), true, {graphics_queue_index_.first},
						   {graphics_queue_index_, transfer_queue_index_});
	}
	if(present_queue_index_ == no_queue_index) {
		present_queue_index_ = find_queue(queue_families, vk::QueueFlags(), true,
										  boost::container::flat_set<queue_family_index_t>(),
										  {graphics_queue_index_, transfer_queue_index_});
	}
	// 3. Assign any suitable queue previous steps failed.
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ = find_queue(queue_families, vk::QueueFlagBits::eTransfer, false);
	}
	if(present_queue_index_ == no_queue_index) {
		present_queue_index_ = find_queue(queue_families, vk::QueueFlags(), true);
	}
	// 4. Fall back to using graphics or compute queue for transfer
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ = find_queue(queue_families, vk::QueueFlagBits::eCompute, false,
										   boost::container::flat_set<queue_family_index_t>(),
										   {graphics_queue_index_, present_queue_index_});
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ = find_queue(queue_families, vk::QueueFlagBits::eGraphics, false,
										   boost::container::flat_set<queue_family_index_t>(),
										   {graphics_queue_index_, present_queue_index_});
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ =
				find_queue(queue_families, vk::QueueFlagBits::eCompute, false,
						   boost::container::flat_set<queue_family_index_t>(), {graphics_queue_index_});
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ =
				find_queue(queue_families, vk::QueueFlagBits::eGraphics, false,
						   boost::container::flat_set<queue_family_index_t>(), {graphics_queue_index_});
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ = find_queue(queue_families, vk::QueueFlagBits::eCompute, false);
	}
	if(transfer_queue_index_ == no_queue_index) {
		transfer_queue_index_ = find_queue(queue_families, vk::QueueFlagBits::eGraphics, false);
	}
}

void device::create_device() {
	std::vector<queue_index_t> queue_indexes{graphics_queue_index_, transfer_queue_index_,
											 present_queue_index_};
	std::sort(queue_indexes.begin(), queue_indexes.end());
	queue_indexes.erase(std::unique(queue_indexes.begin(), queue_indexes.end()), queue_indexes.end());
	std::vector<float> prios(queue_indexes.size(), 1.0f);
	std::vector<vk::DeviceQueueCreateInfo> dev_queue_ci;
	for(auto& qi : queue_indexes) {
		if(dev_queue_ci.empty() || dev_queue_ci.back().queueFamilyIndex != qi.first) {
			vk::DeviceQueueCreateInfo ci;
			ci.queueFamilyIndex = qi.first;
			ci.queueCount = 1;
			ci.pQueuePriorities = prios.data();
			dev_queue_ci.push_back(ci);
		} else {
			dev_queue_ci.back().queueCount++;
		}
	}

	auto req_features = required_device_features();
	vk::PhysicalDeviceFeatures dev_features;
	for(auto& rf : req_features) {
		dev_features.*rf = true;
	}
	vk::DeviceCreateInfo dev_ci;
	const char* swapchain_extension_name = "VK_KHR_swapchain";
	dev_ci.ppEnabledExtensionNames = &swapchain_extension_name;
	dev_ci.enabledExtensionCount = 1;

#ifdef DEBUG
	const char* layer_name = "VK_LAYER_LUNARG_standard_validation";
	dev_ci.ppEnabledLayerNames = &layer_name;
	dev_ci.enabledLayerCount = 1;
#endif

	dev_ci.pEnabledFeatures = &dev_features;
	dev_ci.pQueueCreateInfos = dev_queue_ci.data();
	dev_ci.queueCreateInfoCount = uint32_t(dev_queue_ci.size());

#if VK_HEADER_VERSION == 70 // workaround for a bug in vulkan SDK 1.1.70
	native_device_ =
			vk::UniqueDevice(physical_device_.createDevice(dev_ci), vk::ObjectDestroy<vk::NoParent>());
#else
	native_device_ = physical_device_.createDeviceUnique(dev_ci);
#endif

	graphics_queue_ = native_device_->getQueue(graphics_queue_index_.first, graphics_queue_index_.second);
	transfer_queue_ = native_device_->getQueue(transfer_queue_index_.first, transfer_queue_index_.second);
	present_queue_ = native_device_->getQueue(present_queue_index_.first, present_queue_index_.second);
}

device::~device() {}

boost::optional<vk::Format>
device::best_supported_format_try(vk::ArrayProxy<const vk::Format> candidates,
								  vk::FormatFeatureFlags required_flags,
								  format_support_query_type query_type) const noexcept {
	auto member = (query_type == format_support_query_type::optimal_tiling_image)
						  ? &vk::FormatProperties::optimalTilingFeatures
						  : ((query_type == format_support_query_type::linear_tiling_image)
									 ? &vk::FormatProperties::linearTilingFeatures
									 : &vk::FormatProperties::bufferFeatures);
	auto it = std::find_if(candidates.begin(), candidates.end(),
						   [this, member, required_flags](vk::Format fmt) {
							   const auto& flags = physical_device_.getFormatProperties(fmt).*member;
							   return bool(flags) && ((flags & required_flags) == required_flags);
						   });
	if(it != candidates.end()) {
		return *it;
	} else {
		return {};
	}
}

vk::Format device::best_supported_depth_attachment_format(vk::FormatFeatureFlags additional_required_flags,
														  format_support_query_type query_type) const {
	auto fmt = best_supported_format_try(
			{vk::Format::eD32Sfloat, vk::Format::eX8D24UnormPack32, vk::Format::eD16Unorm},
			vk::FormatFeatureFlagBits::eDepthStencilAttachment | additional_required_flags, query_type);
	if(fmt) {
		return fmt.get();
	} else {
		throw mce::graphics_exception("No supported depth attachment format with the required flags found.");
	}
}

vk::Format
device::best_supported_depth_stencil_attachment_format(vk::FormatFeatureFlags additional_required_flags,
													   format_support_query_type query_type) const {
	auto fmt = best_supported_format_try(
			{vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint},
			vk::FormatFeatureFlagBits::eDepthStencilAttachment | additional_required_flags, query_type);
	if(fmt) {
		return fmt.get();
	} else {
		throw mce::graphics_exception(
				"No supported depth-stencil attachment format with the required flags found.");
	}
}

vk::Format device::best_supported_format(vk::ArrayProxy<const vk::Format> candidates,
										 vk::FormatFeatureFlags required_flags,
										 format_support_query_type query_type) const {
	auto fmt = best_supported_format_try(candidates, required_flags, query_type);
	if(fmt) {
		return fmt.get();
	} else {
		throw mce::graphics_exception(
				"No supported format with the required flags found in the given candidates.");
	}
}

static std::vector<vk::Bool32 vk::PhysicalDeviceFeatures::*> required_device_features_table = {
		{&vk::PhysicalDeviceFeatures::textureCompressionBC, &vk::PhysicalDeviceFeatures::samplerAnisotropy}};

const std::vector<vk::Bool32 vk::PhysicalDeviceFeatures::*>& device::required_device_features() const {
	return required_device_features_table;
}

} /* namespace graphics */
} /* namespace mce */
