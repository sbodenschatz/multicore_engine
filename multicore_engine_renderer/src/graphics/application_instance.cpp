/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/application_instance.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <graphics/application_instance.hpp>
#ifdef MULTICORE_ENGINE_WINDOWS
#include <windows.h> //Fix macro redefinition error in glfw header
#endif				 // MULTICORE_ENGINE_WINDOWS
#include <GLFW/glfw3.h>
#include <algorithm>
#include <core/version.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <util/unused.hpp>
#include <vulkan/vulkan.hpp>

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

std::mutex application_instance::glfw_init;
int application_instance::glfw_refcount{0};

application_instance::application_instance(const std::vector<std::string>& exts,
										   unsigned int validation_level)
		: validation_level(validation_level) {
	{ // Initialize glfw if required:
		std::lock_guard<std::mutex> lock(glfw_init);
		if(glfw_refcount == 0) {
			glfwInit();
		}
		glfw_refcount++;
	}

	if(!glfwVulkanSupported()) throw std::runtime_error("Vulkan not supported.");

	unsigned int extensions_count;
	const char** extensions_cstr = glfwGetRequiredInstanceExtensions(&extensions_count);

	for(unsigned int i = 0; i < extensions_count; ++i) {
		extensions.emplace_back(extensions_cstr[i]);
	}
	std::copy(exts.begin(), exts.end(), std::back_inserter(extensions));
	if(validation_level > 0) {
		layers.emplace_back("VK_LAYER_LUNARG_standard_validation");
		extensions.emplace_back("VK_EXT_debug_report");
	}
	std::sort(extensions.begin(), extensions.end());
	auto new_end = std::unique(extensions.begin(), extensions.end());
	extensions.erase(new_end, extensions.end());

	vk::ApplicationInfo app_info;
	app_info.apiVersion = VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION);
	auto v = core::get_build_version_number();
	app_info.applicationVersion = VK_MAKE_VERSION(v.major, v.minor, v.patch);
	app_info.engineVersion = VK_MAKE_VERSION(v.major, v.minor, v.patch);
	app_info.pApplicationName = "Demo";
	app_info.pEngineName = "mce";

	std::vector<const char*> layer_names;
	std::transform(layers.begin(), layers.end(), std::back_inserter(layer_names),
				   std::mem_fn(&std::string::c_str));
	std::vector<const char*> extension_names;
	std::transform(extensions.begin(), extensions.end(), std::back_inserter(extension_names),
				   std::mem_fn(&std::string::c_str));
	// for(const auto& e : extensions) std::cout << e << std::endl;
	vk::InstanceCreateInfo instance_ci;
	instance_ci.enabledLayerCount = uint32_t(layer_names.size());
	instance_ci.ppEnabledLayerNames = layer_names.data();
	instance_ci.enabledExtensionCount = uint32_t(extension_names.size());
	instance_ci.ppEnabledExtensionNames = extension_names.data();
	instance_ci.pApplicationInfo = &app_info;

	instance_ = vk::createInstance(instance_ci);
	fptr_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
			instance_->getProcAddr("vkCreateDebugReportCallbackEXT"));
	fptr_vkDebugReportMessageEXT =
			reinterpret_cast<PFN_vkDebugReportMessageEXT>(instance_->getProcAddr("vkDebugReportMessageEXT"));
	fptr_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
			instance_->getProcAddr("vkDestroyDebugReportCallbackEXT"));

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
		validation_report_cb = unique_handle<vk::DebugReportCallbackEXT>(
				instance_->createDebugReportCallbackEXT(debug_report_ci),
				[this](vk::DebugReportCallbackEXT& handle,
					   const vk::Optional<const vk::AllocationCallbacks>& alloc) {
					instance_->destroyDebugReportCallbackEXT(handle, alloc);
				});
	}
}

application_instance::~application_instance() {
	// Terminate glfw if we are the last user
	std::lock_guard<std::mutex> lock(glfw_init);
	glfw_refcount--;
	if(glfw_refcount == 0) {
		glfwTerminate();
	}
}

VkBool32 MCE_VK_CALLBACK application_instance::validation_report_callback_static(
		VkDebugReportFlagsEXT flags_, VkDebugReportObjectTypeEXT objectType_, uint64_t object,
		size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage,
		void* pUserData) {
	auto target = static_cast<const application_instance*>(pUserData);
	return target->validation_report_callback(flags_, objectType_, object, location, messageCode,
											  pLayerPrefix, pMessage);
}

VkBool32 application_instance::validation_report_callback(VkDebugReportFlagsEXT flags_,
														  VkDebugReportObjectTypeEXT objectType_,
														  uint64_t object, size_t location,
														  int32_t messageCode, const char* pLayerPrefix,
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
	(*out) << msg;
	out->flush();
#ifdef _MSC_VER
	OutputDebugStringA(msg.c_str());
#endif
	return false;
}

} /* namespace graphics */
} /* namespace mce */
