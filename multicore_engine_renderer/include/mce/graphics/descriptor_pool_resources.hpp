/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_pool_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_POOL_RESOURCES_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_POOL_RESOURCES_HPP_

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <boost/operators.hpp>
#include <mce/containers/generic_flat_map.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>

namespace mce {
namespace graphics {

class descriptor_set_resources
		: public boost::equality_comparable<
				  descriptor_set_resources,
				  boost::additive<descriptor_set_resources,
								  boost::multipliable<descriptor_set_resources, uint32_t>>> {
	template <typename T>
	using sized_small_vector = boost::container::small_vector<T, 16>;

	containers::generic_flat_map<sized_small_vector, vk::DescriptorType, uint32_t> descriptors_;
	uint32_t descriptor_sets_;

public:
	descriptor_set_resources() : descriptor_sets_{0} {}

	explicit descriptor_set_resources(vk::ArrayProxy<const vk::DescriptorPoolSize> descriptors,
									  uint32_t descriptor_sets = 1)
			: descriptor_sets_{descriptor_sets} {
		for(const auto& d : descriptors) {
			descriptors_[d.type] += d.descriptorCount;
		}
	}

	// cppcheck-suppress noExplicitConstructor
	descriptor_set_resources(const descriptor_set_layout& layout, uint32_t descriptor_sets = 1)
			: descriptor_sets_{descriptor_sets} {
		for(const auto& d : layout.bindings()) {
			descriptors_[d.descriptor_type] += descriptor_sets * d.descriptor_count;
		}
	}

	bool operator==(const descriptor_set_resources& other) const {
		return std::tie(descriptors_, descriptor_sets_) ==
			   std::tie(other.descriptors_, other.descriptor_sets_);
	}

	descriptor_set_resources& operator+=(const descriptor_set_resources& other) {
		for(const auto& od : other.descriptors_) {
			descriptors_[od.first] += od.second;
		}
		descriptor_sets_ += other.descriptor_sets_;
		return *this;
	}
	descriptor_set_resources& operator-=(const descriptor_set_resources& other) {
		for(const auto& od : other.descriptors_) {
			descriptors_[od.first] -= od.second;
		}
		descriptor_sets_ -= other.descriptor_sets_;
		return *this;
	}
	descriptor_set_resources& operator*=(uint32_t factor) {
		for(const auto& d : descriptors_) {
			descriptors_[d.first] *= factor;
		}
		descriptor_sets_ *= factor;
		return *this;
	}

	uint32_t min() const {
		if(descriptors_.empty()) return descriptor_sets_;
		return std::min(
				descriptor_sets_,
				std::min_element(descriptors_.begin(), descriptors_.end(), [](const auto& a, const auto& b) {
					return a.second < b.second;
				})->second);
	}

	uint32_t descriptor_sets() const {
		return descriptor_sets_;
	}

	const containers::generic_flat_map<sized_small_vector, vk::DescriptorType, uint32_t>&
	descriptors() const {
		return descriptors_;
	}

	uint32_t descriptors(vk::DescriptorType type) const {
		auto it = descriptors_.find(type);
		if(it == descriptors_.end()) return 0;
		return it->second;
	}

	bool sufficient_for(const descriptor_set_resources& request) const {
		if(descriptor_sets_ < request.descriptor_sets_) return false;
		for(const auto& rd : request.descriptors_) {
			if(descriptors(rd.first) < rd.second) return false;
		}
		return true;
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_DESCRIPTOR_POOL_RESOURCES_HPP_ */
