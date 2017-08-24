/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_HPP_

#include <array>
#include <boost/container/small_vector.hpp>
#include <boost/variant.hpp>
#include <mce/graphics/descriptor_set_deleter.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set_layout;
class device;
class pipeline_layout;
class unique_descriptor_pool;
class descriptor_set;

namespace detail {

template <size_t entries, typename T>
class descriptor_set_updater {
	vk::Device dev_;
	vk::DescriptorSet set_;
	std::array<vk::WriteDescriptorSet, entries> writes_;
	bool last = true;

	friend class descriptor_set_updater<entries + 1, vk::DescriptorImageInfo>;
	friend class descriptor_set_updater<entries + 1, vk::DescriptorBufferInfo>;
	friend class descriptor_set_updater<entries - 1, vk::DescriptorImageInfo>;
	friend class descriptor_set_updater<entries - 1, vk::DescriptorBufferInfo>;
	friend class descriptor_set_updater<entries - 1, void>;

	template <typename U>
	descriptor_set_updater(const descriptor_set_updater<entries - 1, U>& prev, vk::WriteDescriptorSet write)
			: dev_{prev.dev_}, set_{prev.set_} {
		for(size_t i = 0; i + 1 < entries; ++i) {
			writes_[i] = prev.writes_[i];
		}
		writes_[entries - 1] = write;
	}

public:
	descriptor_set_updater<entries + 1, vk::DescriptorImageInfo>
	operator()(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
			   vk::ArrayProxy<const vk::DescriptorImageInfo> data) {
		last = false;
		return {*this, vk::WriteDescriptorSet(set_, binding, array_start_element, data.size(), type,
											  data.data(), nullptr, nullptr)};
	}
	descriptor_set_updater<entries + 1, vk::DescriptorBufferInfo>
	operator()(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
			   vk::ArrayProxy<const vk::DescriptorBufferInfo> data) {
		last = false;
		return {*this, vk::WriteDescriptorSet(set_, binding, array_start_element, data.size(), type, nullptr,
											  data.data(), nullptr)};
	}

	~descriptor_set_updater() noexcept {
		if(last) dev_.updateDescriptorSets(writes_, {});
	}
};

template <>
class descriptor_set_updater<0, void> {

	friend class descriptor_set_updater<1, vk::DescriptorImageInfo>;
	friend class descriptor_set_updater<1, vk::DescriptorBufferInfo>;
	friend class mce::graphics::descriptor_set;

	vk::Device dev_;
	vk::DescriptorSet set_;
	std::array<vk::WriteDescriptorSet, 1> writes_; // Dummy

	descriptor_set_updater(vk::Device dev, vk::DescriptorSet set) : dev_{dev}, set_{set} {}

public:
	descriptor_set_updater<1, vk::DescriptorImageInfo>
	operator()(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
			   vk::ArrayProxy<const vk::DescriptorImageInfo> data) {
		return {*this, vk::WriteDescriptorSet(set_, binding, array_start_element, data.size(), type,
											  data.data(), nullptr, nullptr)};
	}
	descriptor_set_updater<1, vk::DescriptorBufferInfo>
	operator()(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
			   vk::ArrayProxy<const vk::DescriptorBufferInfo> data) {
		return {*this, vk::WriteDescriptorSet(set_, binding, array_start_element, data.size(), type, nullptr,
											  data.data(), nullptr)};
	}
};

} // namespace detail

/// Encapsulates a vulkan descriptor set holding non-attribute data for shaders.
/**
 * The descriptor_set objects allocated from a unique_descriptor_pool are RAII-owners of the underlying
 * descriptor set resource and return them to the pool on destruction.
 * The ones allocated from simple_descriptor_pool are non-owning and become invalid if the pool takes the
 * resources back through reset or pool destruction.
 */
class descriptor_set {
	device* dev_;
	queued_handle<descriptor_set_unique_handle> native_descriptor_set_;

	friend class simple_descriptor_pool;
	friend class unique_descriptor_pool;
	descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set,
				   std::shared_ptr<const descriptor_set_layout> layout);
	descriptor_set(device& dev, vk::DescriptorSet native_descriptor_set, unique_descriptor_pool* pool,
				   destruction_queue_manager* dqm, std::shared_ptr<const descriptor_set_layout> layout);

public:
	/// Describes an element in a container-based batched update of the descriptor_set.
	/**
	 * See equivalent members of vk::WriteDescriptorSet from the vulkan spec for details.
	 */
	struct write_descriptor_set {
		/// The binding to update.
		uint32_t binding;
		/// The array index to start from.
		uint32_t array_start_element;
		/// The type of the descriptors to update.
		vk::DescriptorType type;
		/// The data to update the descriptors with (can either be image or buffer data).
		boost::variant<boost::container::small_vector<vk::DescriptorImageInfo, 16>,
					   boost::container::small_vector<vk::DescriptorBufferInfo, 16>>
				data;
	};

	/// Forbids copying.
	descriptor_set(const descriptor_set&) = delete;
	/// Forbids copying.
	descriptor_set& operator=(const descriptor_set&) = delete;
	/// Allows moving.
	descriptor_set(descriptor_set&&) noexcept = default;
	/// Allows moving.
	descriptor_set& operator=(descriptor_set&&) noexcept = default;

	/// \brief Destroys the descriptor_set and returns the underlying resources back to the pool if it is the
	/// RAII-owner of the resources (allocated from unique_descriptor_pool).
	~descriptor_set();

	/// Allows access to the descriptor_set_layout that the descriptor_set uses.
	/**
	 * May be null if the descriptor_set was allocated from a simple_descriptor_pool and the store_layout
	 * parameter of the allocation member function was false.
	 */
	const std::shared_ptr<const descriptor_set_layout>& layout() const {
		return native_descriptor_set_.get_deleter().layout();
	}

	/// Allows access to the native vulkan descriptor set handle.
	vk::DescriptorSet native_descriptor_set() const {
		return native_descriptor_set_.get();
	}

	/// Updates image-related descriptor data.
	void update_images(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
					   vk::ArrayProxy<const vk::DescriptorImageInfo> data);
	/// Updates buffer-related descriptor data.
	void update_buffers(uint32_t binding, uint32_t array_start_element, vk::DescriptorType type,
						vk::ArrayProxy<const vk::DescriptorBufferInfo> data);

	/// \brief Returns a chainable function object that enables grouping multiple descriptor set writes in a
	/// single API call.
	/**
	 * The returned function object is callable with the signature of update_images as well as the signature
	 * of update_buffers and returns another function object with the same properties.
	 * This way a chain can be formed and after completion the actual update is performed by the last object
	 * in the chain on destruction.
	 *
	 * Example:
	 * @code
	 * descriptor_set ds = ...;
	 * ds.update()
	 * 		(0,0,vk::DescriptorType::eUniformBuffer,{ubo.store(data)})
	 * 		(1,0,vk::DescriptorType::eCombinedImageSampler,{tex.bind()});
	 * @endcode
	 */
	detail::descriptor_set_updater<0, void> update();

	/// \brief Updates multiple bindings in a single operation with data described by the given
	/// write_descriptor_set collection.
	void update(vk::ArrayProxy<const write_descriptor_set> writes);

	/// Binds the given descriptor sets in the given command buffer using the given additional parameters.
	static void bind(vk::CommandBuffer cb, const std::shared_ptr<const pipeline_layout>& layout,
					 uint32_t first_set, vk::ArrayProxy<const descriptor_set> sets,
					 vk::ArrayProxy<const uint32_t> dynamic_offsets = {});
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_HPP_ */
