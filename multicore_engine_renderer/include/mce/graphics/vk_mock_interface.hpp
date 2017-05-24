/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/vk_mock_interface.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_VK_MOCK_INTERFACE_HPP_
#define GRAPHICS_VK_MOCK_INTERFACE_HPP_

/**
 * \file
 * Defines the interface to mock vulkan API calls out of parts of the graphics subsystem for unit tests.
 * For the mocked versions see file /multicore_engine_tests/src/vk_mock_interface.cpp.
 */

#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/unique_handle.hpp>

namespace mce {
namespace graphics {
namespace vk_mock_interface {

/// Returns a bool indicating if the functions are replaced by the mocked version.
bool is_mocked();
/// Wraps allocating device memory using the given device and allocation info.
unique_handle<vk::DeviceMemory, true> allocate_memory(mce::graphics::device* dev, vk::MemoryAllocateInfo& ai);
/// Wraps obtaining the memory properties for the given device.
vk::PhysicalDeviceMemoryProperties get_physical_dev_mem_properties(mce::graphics::device* dev);

} /* namespace vk_mock_interface */
} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_VK_MOCK_INTERFACE_HPP_ */
