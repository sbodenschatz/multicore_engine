/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/destruction_queue_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_
#define MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_

/**
 * \file
 * Defines the destruction queue management functionality.
 */

#include <boost/variant.hpp>
#include <condition_variable>
#include <functional>
#include <mce/containers/scratch_pad_pool.hpp>
#include <mce/graphics/descriptor_set_deleter.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// \brief Provides a ring buffer of queues (one for each index in the frame ring buffer) that are used to
/// defer deletion of resources until asynchronous command execution has finished.
/**
 * Destruction happens by calling the reset member function of the handle without parameters.
 * The destructions in a ring buffer slot happen in the order in which they were enqueued.
 *
 * The queues in the ring buffer can be used in order by switching between them using advance() or can be used
 * in the order of explicitly specified indices using cleanup_and_set_current(). However in the latter case
 * the handle destruction order on destruction of the manager after letting the device complete work on all
 * pending objects deviates from the usage order because it always takes place in ring buffer order.
 *
 * The following types are supported for destruction:
 *   - vk::UniqueBuffer
 *   - vk::UniqueBufferView
 *   - vk::UniqueCommandBuffer
 *   - vk::UniqueCommandPool
 *   - vk::UniqueDescriptorPool
 *   - vk::UniqueDescriptorSet
 *   - vk::UniqueDescriptorSetLayout
 *   - vk::UniqueEvent
 *   - vk::UniqueFence
 *   - vk::UniqueFramebuffer
 *   - vk::UniqueImage
 *   - vk::UniqueImageView
 *   - vk::UniquePipeline
 *   - vk::UniquePipelineLayout
 *   - vk::UniqueQueryPool
 *   - vk::UniqueRenderPass
 *   - vk::UniqueSampler
 *   - vk::UniqueSemaphore
 *   - vk::UniqueShaderModule
 *   - vk::UniqueSurfaceKHR
 *   - vk::UniqueSwapchainKHR
 *   - mce::graphics::device_memory_handle
 *   - mce::graphics::destruction_queue_manager::executor<std::function<void()>>
 *   - std::shared_ptr<void>
 *   - descriptor_set_unique_handle
 */
class destruction_queue_manager {
public:
	/// Wrapper to allow running callables from the destruction queue.
	/**
	 * F must be callable with signature <code>void()</code>.
	 */
	template <typename F>
	struct executor {
		F executee; ///< The callable to be called.
		/// Runs the wrapped function to hook into the destruction queue.
		void reset() {
			executee();
		}
	};

	/// Creates an executor from the given function object.
	// cppcheck-suppress passedByValue
	static executor<std::function<void()>> make_executor(std::function<void()> f) {
		return executor<std::function<void()>>{std::move(f)};
	}

private:
	// cppcheck-suppress copyCtorAndEqOperator
	class element {
		struct reset_visitor : boost::static_visitor<> {
			void operator()(boost::blank&) {}
			template <typename T>
			void operator()(T& handle) {
				handle.reset();
			}
		};

	public:
		boost::variant<boost::blank, vk::UniqueBuffer, vk::UniqueBufferView, vk::UniqueCommandBuffer,
					   vk::UniqueCommandPool, vk::UniqueDescriptorPool, vk::UniqueDescriptorSet,
					   vk::UniqueDescriptorSetLayout, vk::UniqueEvent, vk::UniqueFence, vk::UniqueFramebuffer,
					   vk::UniqueImage, vk::UniqueImageView, vk::UniquePipeline, vk::UniquePipelineLayout,
					   vk::UniqueQueryPool, vk::UniqueRenderPass, vk::UniqueSampler, vk::UniqueSemaphore,
					   vk::UniqueShaderModule, vk::UniqueSurfaceKHR, vk::UniqueSwapchainKHR,
					   device_memory_handle, executor<std::function<void()>>, std::shared_ptr<void>,
					   descriptor_set_unique_handle>
				data;
		template <typename T>
		explicit element(T&& data) : data{std::forward<T>(data)} {}
		element(element&& other) noexcept {
			try {
				data = std::move(other.data);
			} catch(...) {
				data = boost::blank{};
			}
		}
		element& operator=(element&& other) noexcept {
			try {
				data = std::move(other.data);
			} catch(...) {
				data = boost::blank{};
			}
			return *this;
		}
		void reset() {
			reset_visitor v;
			data.apply_visitor(v);
		}
	};
	std::mutex queue_mutex;
	device* dev_;
	std::vector<std::vector<element>> queues;
	uint32_t current_ring_index = 0;
	uint32_t ring_slots;
	containers::scratch_pad_pool<std::vector<element>> temp_pool;
	bool in_cleanup = false;
	std::condition_variable in_cleanup_cv;

public:
	/// Creates a destruction queue manager for the given device and with the given number of queues.
	destruction_queue_manager(device* dev, uint32_t ring_slots)
			: dev_{dev}, queues{ring_slots}, ring_slots{ring_slots} {}
	/// \brief Destroys the destruction queue manager and all pending objects after ensuring completion by
	/// waiting for the device to be idle.
	~destruction_queue_manager();

	/// \brief Inserts the resource managed by the given handle to be destroyed when the current ring index is
	/// cleared.
	template <typename T>
	void enqueue(T&& handle) { // TODO make noexcept if possible because it is used in destructors.
		std::lock_guard<std::mutex> lock(queue_mutex);
		queues[current_ring_index].emplace_back(std::forward<T>(handle));
	}

	/// Advances the manager to the next queue and cleans it.
	void advance();

	/// Cleans the given ring index and sets it as the new current ring index.
	void cleanup_and_set_current(uint32_t ring_index);
};

/// \brief RAII wrapper to hold unique ownership of a resource managed by a T object and release it to an
/// associated destruction_queue_manager when the queued_handle goes out of scope or is reassigned.
template <typename T>
class queued_handle {
	T handle_;
	destruction_queue_manager* qmgr;

public:
	/// Creates an empty queued_handle.
	// cppcheck-suppress uninitMemberVar
	queued_handle() noexcept : qmgr{nullptr} {}
	/// Created a queued_handle from the given resource handle and destruction_queue_manager.
	// cppcheck-suppress uninitMemberVar
	queued_handle(T&& handle, destruction_queue_manager* destruction_queue_mgr) noexcept
			: handle_{std::move(handle)}, qmgr{destruction_queue_mgr} {}
	/// Allows move construction.
	// cppcheck-suppress uninitMemberVar
	queued_handle(queued_handle&& other) noexcept : handle_{std::move(other.handle_)}, qmgr{other.qmgr} {
		other.qmgr = nullptr;
	}
	/// Allows move assignment.
	// cppcheck-suppress operatorEqVarError
	queued_handle& operator=(queued_handle&& other) noexcept {
		if(qmgr) {
			qmgr->enqueue(std::move(handle_));
		}
		handle_ = std::move(other.handle_);
		qmgr = other.qmgr;
		other.qmgr = nullptr;
		return *this;
	}

	/// Explicitly forbids copying as queued_handle is a move-only type for unique ownership.
	queued_handle(const queued_handle&) = delete;
	/// Explicitly forbids copying as queued_handle is a move-only type for unique ownership.
	queued_handle& operator=(const queued_handle&) = delete;

	/// Releases the held resource to the destruction_queue_manager.
	~queued_handle() noexcept {
		if(qmgr) {
			qmgr->enqueue(std::move(handle_));
		}
	}
	/// Checks if the queued_handle is non-empty.
	explicit operator bool() const {
		return handle_.operator bool();
	}
	/// Allows member access to the held resource.
	const T* operator->() const {
		return &handle_;
	}
	/// Allows member access to the held resource.
	T* operator->() {
		return &handle_;
	}
	/// Allows access to the held resource.
	const T& operator*() const {
		return handle_;
	}
	/// Allows access to the held resource.
	T& operator*() {
		return handle_;
	}
	/// Allows access to the held resource.
	T get() const {
		return handle_;
	}
	/// \brief Releases the ownership of the held resource and makes the queued_handle empty, the ownership is
	/// transferred to the returned handle.
	T release() {
		qmgr = nullptr;
		return std::move(handle_);
	}
	/// Allows access to the destruction_queue_manager.
	destruction_queue_manager* destruction_manager() const {
		return qmgr;
	}
};
/// \brief RAII wrapper to hold unique ownership of a resource managed by a vk::unique_handle<T> and release
/// it to an associated destruction_queue_manager when the queued_handle goes out of scope or is reassigned.
template <typename T, typename D>
// cppcheck-suppress copyCtorAndEqOperator
class queued_handle<vk::UniqueHandle<T, D>> {
	vk::UniqueHandle<T, D> handle_;
	destruction_queue_manager* qmgr;

public:
	/// Creates an empty queued_handle.
	queued_handle() noexcept : qmgr{nullptr} {}
	/// Created a queued_handle from the given resource handle and destruction_queue_manager.
	queued_handle(vk::UniqueHandle<T, D>&& handle, destruction_queue_manager* destruction_queue_mgr) noexcept
			: handle_{std::move(handle)}, qmgr{destruction_queue_mgr} {}
	/// Allows move construction.
	queued_handle(queued_handle&& other) noexcept : handle_{std::move(other.handle_)}, qmgr{other.qmgr} {
		other.qmgr = nullptr;
	}
	/// Allows move assignment.
	queued_handle& operator=(queued_handle&& other) noexcept {
		if(qmgr) {
			qmgr->enqueue(std::move(handle_));
		}
		handle_ = std::move(other.handle_);
		qmgr = other.qmgr;
		other.qmgr = nullptr;
		return *this;
	}

	/// Explicitly forbids copying as queued_handle is a move-only type for unique ownership.
	queued_handle(const queued_handle&) = delete;
	/// Explicitly forbids copying as queued_handle is a move-only type for unique ownership.
	queued_handle& operator=(const queued_handle&) = delete;

	/// Releases the held resource to the destruction_queue_manager.
	~queued_handle() noexcept {
		if(qmgr) {
			qmgr->enqueue(std::move(handle_));
		}
	}
	/// Checks if the queued_handle is non-empty.
	explicit operator bool() const {
		return handle_.operator bool();
	}
	/// Allows member access to the held resource.
	const vk::UniqueHandle<T, D>& operator->() const {
		return handle_;
	}
	/// Allows member access to the held resource.
	vk::UniqueHandle<T, D>& operator->() {
		return handle_;
	}
	/// Allows access to the held resource.
	const T& operator*() const {
		return *handle_;
	}
	/// Allows access to the held resource.
	T get() const {
		return handle_.get();
	}
	/// Allows access to the deleter for the held resource.
	const D& get_deleter() const {
		return handle_.getDeleter();
	}
	/// \brief Releases the ownership of the held resource and makes the queued_handle empty, the ownership is
	/// transferred to the returned handle.
	vk::UniqueHandle<T, D> release() {
		qmgr = nullptr;
		return std::move(handle_);
	}
	/// Allows access to the destruction_queue_manager.
	destruction_queue_manager* destruction_manager() const {
		return qmgr;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_ */
