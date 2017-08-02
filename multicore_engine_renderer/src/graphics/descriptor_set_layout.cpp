/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/descriptor_set_layout.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <iterator>
#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

descriptor_set_layout::descriptor_set_layout(const device& dev, destruction_queue_manager* dqm,
											 std::vector<binding_element> bindings)
		: bindings_{std::move(bindings)} {
	if(!std::all_of(bindings_.begin(), bindings_.end(), [](const binding_element& b) {
		   return b.immutable_samplers.empty() || b.immutable_samplers.size() == b.descriptor_count;
	   })) {
		throw mce::graphics_exception("Mismatching immutable samplers count specified.");
	}
	boost::container::small_vector<vk::DescriptorSetLayoutBinding, 32> binding_cis;
	binding_cis.reserve(bindings_.size());
	std::transform(bindings_.begin(), bindings_.end(), std::back_inserter(binding_cis),
				   [](const binding_element& b) {
					   return vk::DescriptorSetLayoutBinding(b.binding, b.descriptor_type, b.descriptor_count,
															 b.stage_flags, b.immutable_samplers.data());
				   });
	native_layout_ = queued_handle<vk::UniqueDescriptorSetLayout>(
			dev.native_device().createDescriptorSetLayoutUnique(
					vk::DescriptorSetLayoutCreateInfo({}, uint32_t(binding_cis.size()), binding_cis.data())),
			dqm);
}

descriptor_set_layout::~descriptor_set_layout() {}

} /* namespace graphics */
} /* namespace mce */
