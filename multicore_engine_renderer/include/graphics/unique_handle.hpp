/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/unique_handle.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_UNIQUE_HANDLE_HPP_
#define GRAPHICS_UNIQUE_HANDLE_HPP_

/**
 * \file
 * Defines the unique_handle RAII wrapper for vulkan object handles.
 */

#include <boost/optional.hpp>
#include <functional>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Provides a RAII wrapper for vkhpp wrapped vulkan handles with a custom destroyer function.
template <typename T, bool Custom_Destroyer = true>
class unique_handle {
private:
	T handle;
	boost::optional<vk::Optional<const vk::AllocationCallbacks>> allocator;
	std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer;

public:
	/// Creates an empty unique_handle.
	unique_handle() : handle() {}
	/// Creates a unique_handle from a raw handle and the associated deleter function.
	unique_handle(const T& val,
				  // cppcheck-suppress passedByValue
				  std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer)
			: handle(val), destroyer(std::move(destroyer)) {}
	/// \brief Creates a unique_handle from a raw handle and the associated deleter function with support for
	/// custom allocators.
	unique_handle(const T& val, const vk::Optional<const vk::AllocationCallbacks>& alloc,
				  // cppcheck-suppress passedByValue
				  std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer)
			: handle(val), allocator(alloc), destroyer(std::move(destroyer)) {}
	/// Disallows copying.
	unique_handle(const unique_handle&) = delete;
	/// Disallows copying.
	unique_handle& operator=(const unique_handle&) = delete;
	/// Allows moving.
	unique_handle(unique_handle&& other) noexcept : handle(other.handle),
													allocator(other.allocator),
													destroyer(other.destroyer) {
		other.handle = T();
		other.allocator = boost::none;
		other.destroyer = std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)>();
	}
	/// Allows moving.
	unique_handle& operator=(unique_handle&& other) noexcept {
		reset();
		handle = other.handle;
		allocator = other.allocator;
		destroyer = other.destroyer;
		other.handle = T();
		other.allocator = boost::none;
		other.destroyer = std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)>();
		return *this;
	}
	/// Releases the handle if one is held.
	~unique_handle() {
		reset();
	}
	/// Releases the handle if one is held.
	void reset() {
		if(handle) {
			if(destroyer) {
				vk::Optional<const vk::AllocationCallbacks> alloc = allocator.value_or(nullptr);
				destroyer(handle, alloc);
			}
		}
		allocator = boost::none;
		handle = T();
	}
	/// Allows access to the encapsulated handle.
	T* operator->() noexcept {
		return &handle;
	}
	/// Allows access to the encapsulated handle.
	const T* operator->() const noexcept {
		return &handle;
	}
	/// Allows access to the encapsulated handle.
	T& operator*() noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	const T& operator*() const noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	T& get() noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	const T& get() const noexcept {
		return handle;
	}
	/// Checks, if a handle is held.
	explicit operator bool() const {
		return bool(handle);
	}
	/// Checks, if no handle is held.
	bool operator!() const {
		return !handle;
	}
};

/// Provides a RAII wrapper for vulkan handles without a custom destroyer function.
template <typename T>
class unique_handle<T, false> {
private:
	T handle;
	boost::optional<vk::Optional<const vk::AllocationCallbacks>> allocator;

public:
	/// Creates an empty unique_handle.
	unique_handle() : handle() {}
	/// Creates a unique_handle from a raw handle.
	unique_handle(const T& val) : handle(val) {}
	/// Creates a unique_handle from a raw handle with support for custom allocators.
	unique_handle(const T& val, const vk::Optional<const vk::AllocationCallbacks>& alloc)
			: handle(val), allocator(alloc) {}
	/// Disallows copying.
	unique_handle(const unique_handle&) = delete;
	/// Disallows copying.
	unique_handle& operator=(const unique_handle&) = delete;
	/// Allows moving.
	unique_handle(unique_handle&& other) noexcept : handle(other.handle), allocator(other.allocator) {
		other.handle = T();
		other.allocator = boost::none;
	}
	/// Allows moving.
	unique_handle& operator=(unique_handle&& other) noexcept {
		reset();
		handle = other.handle;
		allocator = other.allocator;
		other.handle = T();
		other.allocator = boost::none;
		return *this;
	}
	/// Releases the handle if one is held.
	~unique_handle() {
		reset();
	}
	/// Releases the handle if one is held.
	void reset() {
		if(handle) {
			handle.destroy(allocator.value_or(nullptr));
		}
		allocator = boost::none;
		handle = T();
	}
	/// Allows access to the encapsulated handle.
	T* operator->() noexcept {
		return &handle;
	}
	/// Allows access to the encapsulated handle.
	const T* operator->() const noexcept {
		return &handle;
	}
	/// Allows access to the encapsulated handle.
	T& operator*() noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	const T& operator*() const noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	T& get() noexcept {
		return handle;
	}
	/// Allows access to the encapsulated handle.
	const T& get() const noexcept {
		return handle;
	}
	/// Checks, if a handle is held.
	explicit operator bool() const {
		return bool(handle);
	}
	/// Checks, if no handle is held.
	bool operator!() const {
		return !handle;
	}
};

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_UNIQUE_HANDLE_HPP_ */
