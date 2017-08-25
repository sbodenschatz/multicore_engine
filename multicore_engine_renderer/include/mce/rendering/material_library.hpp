/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material_library.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_LIBRARY_HPP_
#define MCE_RENDERING_MATERIAL_LIBRARY_HPP_

#include <atomic>
#include <boost/container/flat_map.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace mce {
namespace rendering {

class material_library : public std::enable_shared_from_this<static_model> {
public:
	/// Represents the status of a material_library.
	enum class state { loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<material_library_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	boost::container::flat_map<std::string, material_description> material_descriptions_;

	void complete_loading(const asset::asset_ptr& lib_asset) noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;

	friend class material_manager;

public:
	/// \brief Creates an material library object with the given name. Should only be used within the
	/// rendering system
	/// but can't be private due to being used in make_shared.
	explicit material_library(const std::string& name) : current_state_{state::loading}, name_{name} {}
	/// \brief Creates an material library object with the given name. Should only be used within the
	/// rendering system
	/// but can't be private due to being used in make_shared.
	explicit material_library(std::string&& name) : current_state_{state::loading}, name_{std::move(name)} {}
	/// Destroys the material_library and releases the underlying resources.
	~material_library();
	/// Forbids copy-construction of material_library.
	material_library(const material_library&) = delete;
	/// Forbids copy-construction of material_library.
	material_library& operator=(const material_library&) = delete;
	/// \brief Instructs the rendering system to run the handler function object when the material library has
	/// completed loading and was transfered to the graphics device or to run the error_handler if an error
	/// occurred during loading or transfer.
	/**
	 * The handler function object must have the signature <code>void(const material_library_ptr& material
	 * library)</code>. The error_handler function object must have the signature
	 * <code>void(std::exception_ptr)</code>. Both handlers are called on the thread calling this function or
	 * on a worker thread of the asset system. Both must fit into their respective handler function wrapper
	 * type material_library_completion_handler and error_handler.
	 */
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Material '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			lock.unlock();
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Material '" + name() + "' was cached as failed.")));
		} else {
			completion_handlers.emplace_back(std::move(handler));
			error_handlers.emplace_back(std::move(error_handler));
		}
	}
	/// Checks if the material library is ready for use.
	bool ready() const noexcept {
		return current_state_ == state::ready;
	}
	/// Checks if an error prevented loading.
	bool has_error() const noexcept {
		return current_state_ == state::error;
	}
	/// Triggers an error check by throwing an exception if an error prevented loading.
	void check_error_flag() const {
		if(current_state_ == state::error)
			throw path_not_found_exception("Error loading material library '" + name_ + "'.");
	}
	/// Returns the current state of the material library.
	state current_state() const noexcept {
		return current_state_;
	}
	/// Returns the name of the material library.
	const std::string& name() const noexcept {
		return name_;
	}
	/// Allows read-access to the material descriptions in this material_library.
	const boost::container::flat_map<std::string, material_description>& material_descriptions() const {
		return material_descriptions_;
	}
	/// \brief Attempts to look up the material with the given name and returns a pointer to it or a nullptr
	/// if it doesn't exist in this library.
	const material_description* find_material_description(const std::string& name) const {
		auto it = material_descriptions_.find(name);
		if(it != material_descriptions_.end()) {
			return &it->second;
		} else {
			return nullptr;
		}
	}
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_LIBRARY_HPP_ */
