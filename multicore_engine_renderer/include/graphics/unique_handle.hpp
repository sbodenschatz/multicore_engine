/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/unique_handle.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_UNIQUE_HANDLE_HPP_
#define GRAPHICS_UNIQUE_HANDLE_HPP_

#include <boost/optional.hpp>
#include <functional>
#include <vulkan/vulkan.hpp>

template <typename T, bool Custom_Destroyer = true>
class unique_handle {
private:
	T handle;
	boost::optional<vk::Optional<const vk::AllocationCallbacks>> allocator;
	std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer;

public:
	unique_handle() : handle() {}
	unique_handle(const T& val,
				  // cppcheck-suppress passedByValue
				  std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer)
			: handle(val), destroyer(std::move(destroyer)) {}
	unique_handle(const T& val, const vk::Optional<const vk::AllocationCallbacks>& alloc,
				  // cppcheck-suppress passedByValue
				  std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)> destroyer)
			: handle(val), allocator(alloc), destroyer(std::move(destroyer)) {}
	unique_handle(const unique_handle&) = delete;
	unique_handle& operator=(const unique_handle&) = delete;
	unique_handle(unique_handle&& other) noexcept : handle(other.handle),
													allocator(other.allocator),
													destroyer(other.destroyer) {
		other.handle = T();
		other.allocator = boost::none;
		other.destroyer = std::function<void(T&, const vk::Optional<const vk::AllocationCallbacks>&)>();
	}
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
	~unique_handle() {
		reset();
	}
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
	T* operator->() noexcept {
		return &handle;
	}
	const T* operator->() const noexcept {
		return &handle;
	}
	T& operator*() noexcept {
		return handle;
	}
	const T& operator*() const noexcept {
		return handle;
	}
	T& get() noexcept {
		return handle;
	}
	const T& get() const noexcept {
		return handle;
	}
	explicit operator bool() const {
		return bool(handle);
	}
	bool operator!() const {
		return !handle;
	}
};

template <typename T>
class unique_handle<T, false> {
private:
	T handle;
	boost::optional<vk::Optional<const vk::AllocationCallbacks>> allocator;

public:
	unique_handle() : handle() {}
	unique_handle(const T& val) : handle(val) {}
	unique_handle(const T& val, const vk::Optional<const vk::AllocationCallbacks>& alloc)
			: handle(val), allocator(alloc) {}
	unique_handle(const unique_handle&) = delete;
	unique_handle& operator=(const unique_handle&) = delete;
	unique_handle(unique_handle&& other) noexcept : handle(other.handle), allocator(other.allocator) {
		other.handle = T();
		other.allocator = boost::none;
	}
	unique_handle& operator=(unique_handle&& other) noexcept {
		reset();
		handle = other.handle;
		allocator = other.allocator;
		other.handle = T();
		other.allocator = boost::none;
		return *this;
	}
	~unique_handle() {
		reset();
	}
	void reset() {
		if(handle) {
			handle.destroy(allocator.value_or(nullptr));
		}
		allocator = boost::none;
		handle = T();
	}
	T* operator->() noexcept {
		return &handle;
	}
	const T* operator->() const noexcept {
		return &handle;
	}
	T& operator*() noexcept {
		return handle;
	}
	const T& operator*() const noexcept {
		return handle;
	}
	T& get() noexcept {
		return handle;
	}
	const T& get() const noexcept {
		return handle;
	}
	explicit operator bool() const {
		return bool(handle);
	}
	bool operator!() const {
		return !handle;
	}
};

#endif /* GRAPHICS_UNIQUE_HANDLE_HPP_ */
