/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/descriptor_set.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/graphics/unique_descriptor_pool.hpp>

namespace mce {
namespace graphics {

descriptor_set::descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set,
							   // cppcheck-suppress passedByValue
							   std::shared_ptr<const descriptor_set_layout> layout)
		: dev_{&dev}, native_descriptor_set_{queued_handle<descriptor_set_unique_handle>(
							  descriptor_set_unique_handle(
									  native_descriptor_set,
									  descriptor_set_deleter(nullptr, std::move(layout))),
							  nullptr)} {}
descriptor_set::descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set,
							   unique_descriptor_pool* pool, destruction_queue_manager* dqm,
							   // cppcheck-suppress passedByValue
							   std::shared_ptr<const descriptor_set_layout> layout)
		: dev_{&dev}, native_descriptor_set_{queued_handle<descriptor_set_unique_handle>(
							  descriptor_set_unique_handle(native_descriptor_set,
														   descriptor_set_deleter(pool, std::move(layout))),
							  dqm)} {}

descriptor_set::~descriptor_set() {}

void descriptor_set::update_images(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
								   vk::ArrayProxy<const vk::DescriptorImageInfo> data) {
	(*dev_)->updateDescriptorSets(
			{vk::WriteDescriptorSet(native_descriptor_set_.get(), binding, array_start_element, data.size(),
									type, data.data(), nullptr, nullptr)},
			{});
}
void descriptor_set::update_buffers(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
									vk::ArrayProxy<const vk::DescriptorBufferInfo> data) {
	(*dev_)->updateDescriptorSets(
			{vk::WriteDescriptorSet(native_descriptor_set_.get(), binding, array_start_element, data.size(),
									type, nullptr, data.data(), nullptr)},
			{});
}

void descriptor_set::bind(vk::CommandBuffer cb, const pipeline_layout& layout, uint32_t first_set,
						  vk::ArrayProxy<const descriptor_set> sets,
						  vk::ArrayProxy<const uint32_t> dynamic_offsets) {
	boost::container::small_vector<vk::DescriptorSet, 8> native_sets;
	native_sets.reserve(sets.size());
	std::transform(sets.begin(), sets.end(), std::back_inserter(native_sets),
				   [](const descriptor_set& set) { return set.native_descriptor_set(); });
	cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout.native_layout(), first_set,
						  {uint32_t(native_sets.size()), native_sets.data()}, dynamic_offsets);
}

void descriptor_set_deleter::operator()(vk::DescriptorSet set) const {
	if(unique_pool_) {
		unique_pool_->free(set, layout_);
	}
	// unique_pool==false -> Non-owning descriptor set.
}

detail::descriptor_set_updater<0, void> descriptor_set::update() {
	return detail::descriptor_set_updater<0, void>(dev_->native_device(), native_descriptor_set_.get());
}

void descriptor_set::update(vk::ArrayProxy<const write_descriptor_set> writes) {
	boost::container::small_vector<vk::WriteDescriptorSet, 64> writes_transformed;
	writes_transformed.reserve(writes.size());
	std::transform(
			writes.begin(), writes.end(), std::back_inserter(writes_transformed),
			[this](const write_descriptor_set& wds) {
				struct v_t : boost::static_visitor<vk::WriteDescriptorSet> {
					vk::DescriptorSet set;
					uint32_t binding;
					uint32_t array_start_element;
					vk::DescriptorType type;
					v_t(vk::DescriptorSet set, uint32_t binding, uint32_t array_start_element,
						vk::DescriptorType type)
							: set{set}, binding{binding},
							  array_start_element{array_start_element}, type{type} {}
					vk::WriteDescriptorSet
					operator()(const boost::container::small_vector<vk::DescriptorImageInfo, 16>& di) {
						return vk::WriteDescriptorSet(set, binding, array_start_element, uint32_t(di.size()),
													  type, di.data(), nullptr, nullptr);
					}
					vk::WriteDescriptorSet
					operator()(const boost::container::small_vector<vk::DescriptorBufferInfo, 16>& di) {
						return vk::WriteDescriptorSet(set, binding, array_start_element, uint32_t(di.size()),
													  type, nullptr, di.data(), nullptr);
					}
				};
				v_t v(native_descriptor_set_.get(), wds.binding, wds.array_start_element, wds.type);
				return wds.data.apply_visitor(v);
			});
	(*dev_)->updateDescriptorSets(vk::ArrayProxy<const vk::WriteDescriptorSet>(
										  uint32_t(writes_transformed.size()), writes_transformed.data()),
								  {});
}

} /* namespace graphics */
} /* namespace mce */
