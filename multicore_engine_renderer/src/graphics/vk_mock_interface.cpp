/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/graphics/vk_mock_interface.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/vk_mock_interface.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {
namespace vk_mock_interface {

bool is_mocked() {
	return false;
}

device_memory_wrapper allocate_memory(mce::graphics::device* dev, vk::MemoryAllocateInfo& ai) {
	if(!dev) throw std::logic_error("device pointer null");
	vk::DeviceMemory dev_mem;
	vk::Result res = dev->native_device().allocateMemory(&ai, nullptr, &dev_mem);
	if(res != vk::Result::eSuccess)
		return vk::UniqueDeviceMemory(vk::DeviceMemory(), vk::DeviceMemoryDeleter(dev->native_device()));
	else
		return vk::UniqueDeviceMemory(dev_mem, vk::DeviceMemoryDeleter(dev->native_device()));
}

vk::PhysicalDeviceMemoryProperties get_physical_dev_mem_properties(mce::graphics::device* dev) {
	if(!dev) throw std::logic_error("device pointer null");
	return dev->physical_device().getMemoryProperties();
}

} /* namespace vk_mock_interface */
} /* namespace graphics */
} /* namespace mce */
