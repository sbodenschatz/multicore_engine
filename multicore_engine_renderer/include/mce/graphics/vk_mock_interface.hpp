/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/vk_mock_interface.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_VK_MOCK_INTERFACE_HPP_
#define GRAPHICS_VK_MOCK_INTERFACE_HPP_

#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/unique_handle.hpp>

namespace mce {
namespace graphics {
namespace vk_mock_interface {

bool is_mocked();
unique_handle<vk::DeviceMemory, true> allocate_memory(mce::graphics::device* dev, vk::MemoryAllocateInfo& ai);
vk::PhysicalDeviceMemoryProperties get_physical_dev_mem_properties(mce::graphics::device* dev);

} /* namespace vk_mock_interface */
} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_VK_MOCK_INTERFACE_HPP_ */
