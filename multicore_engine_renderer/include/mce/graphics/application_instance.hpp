/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/application_instance.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_APPLICATION_INSTANCE_HPP_
#define GRAPHICS_APPLICATION_INSTANCE_HPP_

/**
 * \file
 * Defines the instance level resource management of the application.
 */

#include <atomic>
#include <mce/glfw/instance.hpp>
#include <mutex>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Encapsulates the vulkan instance ralated aspects of the application (extensions, layers, callbacks).
class application_instance {
private:
	glfw::instance glfw_instance;
	std::vector<std::string> layers;
	std::vector<std::string> extensions;
	vk::UniqueInstance instance_;
	vk::UniqueDebugReportCallbackEXT validation_report_cb;
#ifdef DEBUG
	static const unsigned int default_validation_level = 5;
#else
	static const unsigned int default_validation_level = 0;
#endif
	unsigned int validation_level;
	mutable std::mutex validation_log_mtx;

private:
	static VkBool32 VKAPI_CALL
	validation_report_callback_static(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
									  uint64_t object, size_t location, int32_t messageCode,
									  const char* pLayerPrefix, const char* pMessage, void* pUserData);
	VkBool32 validation_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
										uint64_t object, size_t location, int32_t messageCode,
										const char* pLayerPrefix, const char* pMessage) const;

public:
	/// Constructs an application_instance with the given additional extensions and validation level.
	/**
	 * The given extensions are requested additionally to the ones required by glfw.
	 * The validation level determines which debug callback messages are given to the debug output ranging
	 * from 1 for error only to 5 for debug and above (all).
	 */
	application_instance(const std::vector<std::string>& extensions = {},
						 unsigned int validation_level = default_validation_level);
	/// Releases the instance resources.
	~application_instance();

	/// Returns the vulkan instance managed by the application instance.
	const vk::Instance& instance() const {
		return *instance_;
	}

	/// Returns the vulkan instance managed by the application instance.
	vk::Instance instance() {
		return instance_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_APPLICATION_INSTANCE_HPP_ */
