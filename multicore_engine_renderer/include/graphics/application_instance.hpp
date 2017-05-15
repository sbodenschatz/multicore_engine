/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/application_instance.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_APPLICATION_INSTANCE_HPP_
#define GRAPHICS_APPLICATION_INSTANCE_HPP_

#include "unique_handle.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

#ifdef _MSC_VER
#define MCE_VK_CALLBACK __stdcall
#else
#define MCE_VK_CALLBACK
#endif

namespace mce {
namespace graphics {

class application_instance {
private:
	static std::mutex glfw_init;
	static int glfw_refcount;
	std::vector<std::string> layers;
	std::vector<std::string> extensions;
	unique_handle<vk::Instance, false> instance_;
	unique_handle<vk::DebugReportCallbackEXT> validation_report_cb;
#ifdef DEBUG
	static const unsigned int default_validation_level = 5;
#else
	static const unsigned int default_validation_level = 0;
#endif
	unsigned int validation_level;
	mutable std::mutex validation_log_mtx;

private:
	static VkBool32 MCE_VK_CALLBACK validation_report_callback_static(VkDebugReportFlagsEXT flags,
																	  VkDebugReportObjectTypeEXT objectType,
																	  uint64_t object, size_t location,
																	  int32_t messageCode,
																	  const char* pLayerPrefix,
																	  const char* pMessage, void* pUserData);
	VkBool32 validation_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
										uint64_t object, size_t location, int32_t messageCode,
										const char* pLayerPrefix, const char* pMessage) const;

public:
	application_instance(const std::vector<std::string>& extensions = {},
						 unsigned int validation_level = default_validation_level);
	~application_instance();

	const vk::Instance& instance() const {
		return instance_.get();
	}
	vk::Instance& instance() {
		return instance_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_APPLICATION_INSTANCE_HPP_ */
