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

#include <boost/variant.hpp>
#include <mce/graphics/device_memory_manager.hpp>

namespace mce {
namespace graphics {
namespace vk_mock_interface {

struct fake_device_memory_deleter {
	void operator()(vk::DeviceMemory) {}
};

using fake_unique_device_memory = vk::UniqueHandle<vk::DeviceMemory, fake_device_memory_deleter>;

class device_memory_wrapper {
	boost::variant<vk::UniqueDeviceMemory, fake_unique_device_memory> handle_;

public:
	device_memory_wrapper(vk::UniqueDeviceMemory&& m) : handle_(std::move(m)){};
	device_memory_wrapper(fake_unique_device_memory&& m) : handle_(std::move(m)){};

	const vk::DeviceMemory& get() const {
		struct visitor : boost::static_visitor<const vk::DeviceMemory&> {
			const vk::DeviceMemory& operator()(const vk::UniqueDeviceMemory& m) const {
				return *m;
			}
			const vk::DeviceMemory& operator()(const fake_unique_device_memory& m) const {
				return *m;
			}
		};
		visitor v;
		return handle_.apply_visitor(v);
	}
	vk::DeviceMemory get() {
		struct visitor : boost::static_visitor<vk::DeviceMemory> {
			vk::DeviceMemory operator()(const vk::UniqueDeviceMemory& m) const {
				return m.get();
			}
			vk::DeviceMemory operator()(const fake_unique_device_memory& m) const {
				return m.get();
			}
		};
		visitor v;
		return handle_.apply_visitor(v);
	}
};

/// Returns a bool indicating if the functions are replaced by the mocked version.
bool is_mocked();
/// Wraps allocating device memory using the given device and allocation info.
unique_handle<vk::DeviceMemory> allocate_memory(mce::graphics::device* dev, vk::MemoryAllocateInfo& ai);
/// Wraps obtaining the memory properties for the given device.
vk::PhysicalDeviceMemoryProperties get_physical_dev_mem_properties(mce::graphics::device* dev);

} /* namespace vk_mock_interface */
} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_VK_MOCK_INTERFACE_HPP_ */
