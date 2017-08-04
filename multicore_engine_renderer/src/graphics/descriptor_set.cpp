/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/descriptor_set.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline_layout.hpp>

namespace mce {
namespace graphics {

descriptor_set::descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set,
							   std::shared_ptr<descriptor_set_layout> layout)
		: dev_{&dev}, native_descriptor_set_{native_descriptor_set}, layout_{std::move(layout)} {}
descriptor_set::descriptor_set(device& dev, destruction_queue_manager* dqm,
							   vk::UniqueDescriptorSet native_descriptor_set,
							   std::shared_ptr<descriptor_set_layout> layout)
		: dev_{&dev}, descriptor_set_unique{queued_handle<vk::UniqueDescriptorSet>(
							  std::move(native_descriptor_set), dqm)},
		  native_descriptor_set_{native_descriptor_set.get()}, layout_{std::move(layout)} {}

descriptor_set::descriptor_set(descriptor_set&& other) noexcept
		: dev_{other.dev_}, descriptor_set_unique{std::move(other.descriptor_set_unique)},
		  native_descriptor_set_{std::move(other.native_descriptor_set_)}, layout_{std::move(other.layout_)} {
	other.native_descriptor_set_ = nullptr;
	other.dev_ = nullptr;
}
descriptor_set& descriptor_set::operator=(descriptor_set&& other) noexcept {
	dev_ = other.dev_;
	descriptor_set_unique = std::move(other.descriptor_set_unique);
	native_descriptor_set_ = std::move(other.native_descriptor_set_);
	layout_ = std::move(other.layout_);
	other.native_descriptor_set_ = nullptr;
	other.dev_ = nullptr;
	return *this;
}

descriptor_set::~descriptor_set() {}

void descriptor_set::update_images(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
								   vk::ArrayProxy<const vk::DescriptorImageInfo> data) {
	(*dev_)->updateDescriptorSets(
			{vk::WriteDescriptorSet(native_descriptor_set_, binding, array_start_element, data.size(), type,
									data.data(), nullptr, nullptr)},
			{});
}
void descriptor_set::update_buffers(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
									vk::ArrayProxy<const vk::DescriptorBufferInfo> data) {
	(*dev_)->updateDescriptorSets(
			{vk::WriteDescriptorSet(native_descriptor_set_, binding, array_start_element, data.size(), type,
									nullptr, data.data(), nullptr)},
			{});
}

void descriptor_set::bind(vk::CommandBuffer cb, const std::shared_ptr<pipeline_layout>& layout,
						  uint32_t first_set, vk::ArrayProxy<const descriptor_set> sets,
						  vk::ArrayProxy<const uint32_t> dynamic_offsets) {
	boost::container::small_vector<vk::DescriptorSet, 8> native_sets;
	native_sets.reserve(sets.size());
	std::transform(sets.begin(), sets.end(), std::back_inserter(native_sets),
				   [](const descriptor_set& set) { return set.native_descriptor_set(); });
	cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout->native_layout(), first_set,
						  {uint32_t(native_sets.size()), native_sets.data()}, dynamic_offsets);
}

} /* namespace graphics */
} /* namespace mce */
