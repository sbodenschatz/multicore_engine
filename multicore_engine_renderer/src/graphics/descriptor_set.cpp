/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/descriptor_set.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/descriptor_set.hpp>

namespace mce {
namespace graphics {

descriptor_set::descriptor_set(vk::DescriptorSet native_descriptor_set,
							   std::shared_ptr<descriptor_set_layout> layout)
		: native_descriptor_set_{native_descriptor_set}, layout_{std::move(layout)} {}
descriptor_set::descriptor_set(destruction_queue_manager* dqm, vk::UniqueDescriptorSet native_descriptor_set,
							   std::shared_ptr<descriptor_set_layout> layout)
		: descriptor_set_unique{queued_handle<vk::UniqueDescriptorSet>(std::move(native_descriptor_set),
																	   dqm)},
		  native_descriptor_set_{native_descriptor_set.get()}, layout_{std::move(layout)} {}

descriptor_set::~descriptor_set() {}

} /* namespace graphics */
} /* namespace mce */
