/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/unique_handle.hpp
 * Copyright 2016-2018 by Stefan Bodenschatz
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

/// Provides a RAII wrapper for vkhpp wrapped vulkan handles with a custom deleter.
template <typename T, typename D>
class unique_handle {
private:
	D deleter_;
	T handle_;

	void destroy() noexcept {
		if(handle_) {
			deleter_(handle_);
		}
	}

public:
	/// Creates an empty unique_handle.
	unique_handle() = default;
	/// Creates a unique_handle from a raw handle and the associated deleter function.
	unique_handle(const T& handle, const D& deleter) : deleter_(deleter), handle_(handle) {}

	/// Disallows copying.
	unique_handle(const unique_handle&) = delete;
	/// Disallows copying.
	unique_handle& operator=(const unique_handle&) = delete;

	/// Allows moving.
	unique_handle(unique_handle&& other) noexcept
			: deleter_(std::move(other.deleter_)), handle_(other.release()) {}

	/// Allows moving.
	unique_handle& operator=(unique_handle&& other) noexcept {
		destroy();
		deleter_ = std::move(other.deleter_);
		handle_ = other.release();
		return *this;
	}
	/// Destroys the object referenced by the handle if one is held.
	~unique_handle() {
		destroy();
	}
	/// Destroys the object referenced by the handle if one is held.
	void reset() {
		*this = unique_handle();
	}
	/// Allows access to the encapsulated handle.
	T* operator->() noexcept {
		return &handle_;
	}
	/// Allows access to the encapsulated handle.
	const T* operator->() const noexcept {
		return &handle_;
	}
	/// Allows access to the encapsulated handle.
	T& operator*() noexcept {
		return handle_;
	}
	/// Allows access to the encapsulated handle.
	const T& operator*() const noexcept {
		return handle_;
	}
	/// Allows access to the encapsulated handle.
	T& get() noexcept {
		return handle_;
	}
	/// Allows access to the encapsulated handle.
	const T& get() const noexcept {
		return handle_;
	}
	/// Checks, if a handle is held.
	explicit operator bool() const {
		return bool(handle_);
	}
	/// Checks, if no handle is held.
	bool operator!() const {
		return !handle_;
	}

	const D& deleter() const {
		return deleter_;
	}

	D& deleter() {
		return deleter_;
	}

	T release() {
		T res = std::move(handle_);
		handle_ = T();
		deleter_ = D();
		return res;
	}

	void swap(unique_handle& other) {
		using std::swap;
		swap(handle_, other.handle);
		swap(deleter_, other.deleter_);
	}

	friend void swap(unique_handle& lhs, unique_handle& rhs) {
		lhs.swap(rhs);
	}
};

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_UNIQUE_HANDLE_HPP_ */
