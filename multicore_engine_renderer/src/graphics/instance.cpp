/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/instance.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#include <intrin.h>
#include <windows.h> //Required for OutputDebugStringA
#endif
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <glm/glm.hpp>
#include <iostream>
#include <iterator>
#include <mce/core/version.hpp>
#include <mce/exceptions.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/util/unused.hpp>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vulkan/vulkan.hpp>

#if(!defined(GLM_DEPTH_CLIP_SPACE) || GLM_DEPTH_CLIP_SPACE != GLM_DEPTH_ZERO_TO_ONE) &&                      \
		((GLM_CONFIG_CLIP_CONTROL & GLM_CLIP_CONTROL_ZO_BIT) == 0)
#error "A GLM version supporting GLM_FORCE_DEPTH_ZERO_TO_ONE is required for vulkan."
#endif

// Provide wrapper functions to call into extension function pointers because extension functions aren't
// provided by the loader library. To make the symbols available they are defined here.
extern "C" {
PFN_vkCreateDebugReportCallbackEXT fptr_vkCreateDebugReportCallbackEXT;
PFN_vkDebugReportMessageEXT fptr_vkDebugReportMessageEXT;
PFN_vkDestroyDebugReportCallbackEXT fptr_vkDestroyDebugReportCallbackEXT;

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
							   const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	return fptr_vkCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
														   VkDebugReportCallbackEXT callback,
														   const VkAllocationCallbacks* pAllocator) {
	return fptr_vkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags,
												   VkDebugReportObjectTypeEXT objectType, uint64_t object,
												   size_t location, int32_t messageCode,
												   const char* pLayerPrefix, const char* pMessage) {
	return fptr_vkDebugReportMessageEXT(instance, flags, objectType, object, location, messageCode,
										pLayerPrefix, pMessage);
}
}

namespace mce {
namespace graphics {

instance::instance(const core::software_metadata& engine_metadata,
				   const core::software_metadata& application_metadata, const std::vector<std::string>& exts,
				   unsigned int validation_level)
		: validation_level(validation_level) {

	if(!glfw_instance.vulkan_supported())
		throw vulkan_not_supported_exception("Vulkan API is not supported on this system.");

	extensions = glfw_instance.required_vulkan_instance_extensions();

	auto supported_layers_props = vk::enumerateInstanceLayerProperties();
	std::unordered_set<std::string> supported_layer_names;
	std::transform(supported_layers_props.begin(), supported_layers_props.end(),
				   std::inserter(supported_layer_names, supported_layer_names.end()),
				   [](const auto& prop) { return prop.layerName; });

	std::copy(exts.begin(), exts.end(), std::back_inserter(extensions));
	if(validation_level > 0) {
		std::string validation_layer_name = "VK_LAYER_LUNARG_standard_validation";
		if(supported_layer_names.find(validation_layer_name) != supported_layer_names.end()) {
			layers.emplace_back(validation_layer_name);
		}
		extensions.emplace_back("VK_EXT_debug_report");
	}
	std::sort(extensions.begin(), extensions.end());
	auto new_end = std::unique(extensions.begin(), extensions.end());
	extensions.erase(new_end, extensions.end());

	vk::ApplicationInfo app_info;
	app_info.apiVersion = VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION);
	auto& av = application_metadata.version;
	auto& ev = engine_metadata.version;
	app_info.applicationVersion = VK_MAKE_VERSION(av.major, av.minor, av.patch);
	app_info.engineVersion = VK_MAKE_VERSION(ev.major, ev.minor, ev.patch);
	app_info.pApplicationName = application_metadata.name.c_str();
	app_info.pEngineName = engine_metadata.name.c_str();

	std::vector<const char*> layer_names;
	layer_names.reserve(layers.size());
	std::transform(layers.begin(), layers.end(), std::back_inserter(layer_names),
				   [](const std::string& n) { return n.c_str(); });
	std::vector<const char*> extension_names;
	extension_names.reserve(extensions.size());
	std::transform(extensions.begin(), extensions.end(), std::back_inserter(extension_names),
				   [](const std::string& n) { return n.c_str(); });
	// for(const auto& e : extensions) std::cout << e << std::endl;
	vk::InstanceCreateInfo instance_ci;
	instance_ci.enabledLayerCount = uint32_t(layer_names.size());
	instance_ci.ppEnabledLayerNames = layer_names.data();
	instance_ci.enabledExtensionCount = uint32_t(extension_names.size());
	instance_ci.ppEnabledExtensionNames = extension_names.data();
	instance_ci.pApplicationInfo = &app_info;

	native_instance_ = vk::createInstanceUnique(instance_ci);
	fptr_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
			native_instance_->getProcAddr("vkCreateDebugReportCallbackEXT"));
	fptr_vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(
			native_instance_->getProcAddr("vkDebugReportMessageEXT"));
	fptr_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
			native_instance_->getProcAddr("vkDestroyDebugReportCallbackEXT"));

	if(validation_level > 0) {
		vk::DebugReportFlagsEXT validation_report_levels =
				vk::DebugReportFlagBitsEXT::eError; // level 1 = errors only
		if(validation_level >= 5) validation_report_levels |= vk::DebugReportFlagBitsEXT::eDebug;
		if(validation_level >= 4) validation_report_levels |= vk::DebugReportFlagBitsEXT::eInformation;
		if(validation_level >= 3) validation_report_levels |= vk::DebugReportFlagBitsEXT::ePerformanceWarning;
		if(validation_level >= 2) validation_report_levels |= vk::DebugReportFlagBitsEXT::eWarning;
		PFN_vkDebugReportCallbackEXT cb = &validation_report_callback_static;
		vk::DebugReportCallbackCreateInfoEXT debug_report_ci(validation_report_levels, cb,
															 static_cast<void*>(this));
		validation_report_cb = native_instance_->createDebugReportCallbackEXTUnique(debug_report_ci);
	}
}

instance::~instance() {}

VkBool32 VKAPI_CALL instance::validation_report_callback_static(VkDebugReportFlagsEXT flags_,
																VkDebugReportObjectTypeEXT objectType_,
																uint64_t object, size_t location,
																int32_t messageCode, const char* pLayerPrefix,
																const char* pMessage, void* pUserData) {
	auto target = static_cast<const instance*>(pUserData);
	return target->validation_report_callback(flags_, objectType_, object, location, messageCode,
											  pLayerPrefix, pMessage);
}

VkBool32 instance::validation_report_callback(VkDebugReportFlagsEXT flags_,
											  VkDebugReportObjectTypeEXT objectType_, uint64_t object,
											  size_t location, int32_t messageCode, const char* pLayerPrefix,
											  const char* pMessage) const {
	vk::DebugReportFlagsEXT flags = vk::DebugReportFlagBitsEXT(flags_);
	vk::DebugReportObjectTypeEXT object_type = static_cast<vk::DebugReportObjectTypeEXT>(objectType_);
	vk::DebugReportFlagsEXT crit_report_levels = vk::DebugReportFlagBitsEXT::eError;
	crit_report_levels |= vk::DebugReportFlagBitsEXT::eWarning;
	crit_report_levels |= vk::DebugReportFlagBitsEXT::ePerformanceWarning;

	std::stringstream tmpstream;
	tmpstream << vk::to_string(flags) << " " << vk::to_string(object_type) << " [obj=" << object
			  << " loc=" << location << " mc=" << messageCode << " layer=" << pLayerPrefix << "] " << pMessage
			  << std::endl;
	auto msg = tmpstream.str();
	std::ostream* out = nullptr;
	if(flags & crit_report_levels) {
		out = &std::cerr;
	} else {
		out = &std::cout;
	}
	std::lock_guard<std::mutex> lock(validation_log_mtx);
#ifdef _MSC_VER
	OutputDebugStringA(msg.c_str());
#ifdef DEBUG
	if(flags & crit_report_levels) __debugbreak();
#else
	(*out) << msg;
	out->flush();
#endif // DEBUG
#else
	(*out) << msg;
	out->flush();
#endif
	return false;
}

} /* namespace graphics */
} /* namespace mce */
