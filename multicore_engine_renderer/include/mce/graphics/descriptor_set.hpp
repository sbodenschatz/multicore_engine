/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_HPP_

#include <boost/optional.hpp>
#include <mce/graphics/descriptor_set_deleter.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set_layout;
class device;
class pipeline_layout;
class unique_descriptor_pool;

class descriptor_set {
	device* dev_;
	queued_handle<descriptor_set_unique_handle> native_descriptor_set_;

	friend class simple_descriptor_pool;
	friend class unique_descriptor_pool;
	descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set,
				   std::shared_ptr<descriptor_set_layout> layout);
	descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set, unique_descriptor_pool* pool,
				   destruction_queue_manager* dqm, std::shared_ptr<descriptor_set_layout> layout);

public:
	descriptor_set(const descriptor_set&) = delete;
	descriptor_set& operator=(const descriptor_set&) = delete;
	descriptor_set(descriptor_set&&) noexcept = default;
	descriptor_set& operator=(descriptor_set&&) noexcept = default;

	~descriptor_set();

	const std::shared_ptr<descriptor_set_layout>& layout() const {
		return native_descriptor_set_.get_deleter().layout();
	}

	vk::DescriptorSet native_descriptor_set() const {
		return native_descriptor_set_.get();
	}

	void update_images(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
					   vk::ArrayProxy<const vk::DescriptorImageInfo> data);
	void update_buffers(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
						vk::ArrayProxy<const vk::DescriptorBufferInfo> data);

	static void bind(vk::CommandBuffer cb, const std::shared_ptr<pipeline_layout>& layout, uint32_t first_set,
					 vk::ArrayProxy<const descriptor_set> sets,
					 vk::ArrayProxy<const uint32_t> dynamic_offsets = {});
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_HPP_ */
