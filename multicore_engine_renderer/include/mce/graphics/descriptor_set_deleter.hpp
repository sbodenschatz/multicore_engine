/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set_deleter.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_

#include <memory>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class unique_descriptor_pool;
class descriptor_set_layout;

class descriptor_set_deleter {
	unique_descriptor_pool* unique_pool_;
	std::shared_ptr<descriptor_set_layout> layout_;

public:
	descriptor_set_deleter(unique_descriptor_pool* unique_pool, std::shared_ptr<descriptor_set_layout> layout)
			: unique_pool_{unique_pool}, layout_{std::move(layout)} {}

	unique_descriptor_pool* unique_pool() const {
		return unique_pool_;
	}
	const std::shared_ptr<descriptor_set_layout>& layout() const {
		return layout_;
	}

	void operator()(vk::DescriptorSet set) const;
};

using descriptor_set_unique_handle = vk::UniqueHandle<vk::DescriptorSet, descriptor_set_deleter>;

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_ */
