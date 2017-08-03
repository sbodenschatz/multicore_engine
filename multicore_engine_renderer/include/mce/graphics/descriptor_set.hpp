/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_HPP_

#include <boost/optional.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set_layout;

class descriptor_set {
	boost::optional<queued_handle<vk::UniqueDescriptorSet>> descriptor_set_unique;
	vk::DescriptorSet native_descriptor_set_;
	std::shared_ptr<descriptor_set_layout> layout_;

public:
	// cppcheck-suppress passedByValue
	descriptor_set(vk::DescriptorSet native_descriptor_set, std::shared_ptr<descriptor_set_layout> layout);
	// cppcheck-suppress passedByValue
	descriptor_set(destruction_queue_manager* dqm, vk::UniqueDescriptorSet native_descriptor_set,
				   std::shared_ptr<descriptor_set_layout> layout);

	descriptor_set(const descriptor_set&) = delete;
	descriptor_set& operator=(const descriptor_set&) = delete;
	descriptor_set(descriptor_set&&) noexcept = default;
	descriptor_set& operator=(descriptor_set&&) noexcept = default;

	~descriptor_set();

	bool is_owner() const {
		return bool(descriptor_set_unique);
	}

	const std::shared_ptr<descriptor_set_layout>& layout() const {
		return layout_;
	}

	vk::DescriptorSet native_descriptor_set() const {
		return native_descriptor_set_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_HPP_ */
