/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set_deleter.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_

/**
 * \file
 * Defines deleter helpers for descriptor sets allocated from a unique_descriptor_pool.
 */

#include <memory>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class unique_descriptor_pool;
class descriptor_set_layout;

/// Custom deleter class for descriptor sets in vk::UniqueHandle allocated from a unique_descriptor_pool.
class descriptor_set_deleter {
	unique_descriptor_pool* unique_pool_;
	std::shared_ptr<const descriptor_set_layout> layout_;

public:
	/// \brief Creates a deleter to free a descriptor_set using the given layout back to the given
	/// unique_descriptor_pool.
	// cppcheck-suppress passedByValue
	descriptor_set_deleter(unique_descriptor_pool* unique_pool,
						   std::shared_ptr<const descriptor_set_layout> layout)
			: unique_pool_{unique_pool}, layout_{std::move(layout)} {}

	/// Returns the associated unique_descriptor_pool.
	unique_descriptor_pool* unique_pool() const {
		return unique_pool_;
	}
	/// Returns the associated descriptor_set_layout.
	const std::shared_ptr<const descriptor_set_layout>& layout() const {
		return layout_;
	}

	/// Performs the free operation against the associated unique_descriptor_pool.
	void operator()(vk::DescriptorSet set) const;
};

/// Unique handle instance for descriptor sets allocated from a unique_descriptor_pool.
using descriptor_set_unique_handle = vk::UniqueHandle<vk::DescriptorSet, descriptor_set_deleter>;

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_DELETER_HPP_ */
