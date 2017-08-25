/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_HPP_
#define MCE_RENDERING_MATERIAL_HPP_

#include <atomic>
#include <boost/container/flat_map.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace mce {
namespace rendering {
class material_manager;

class material : public std::enable_shared_from_this<material> {
	graphics::texture_ptr albedo_map_;
	graphics::texture_ptr normal_map_;
	graphics::texture_ptr material_map_; // Metallic,Roughness,Ambient occlusion
	graphics::texture_ptr emission_map_;

public:
	/// Represents the status of a material.
	enum class state { loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<material_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	boost::container::flat_map<std::string, material_description> material_descriptions_;

	void texture_loaded(const graphics::texture_ptr& tex) noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;

	friend class material_manager;

public:
	/// \brief Creates an material object for the given description. Should only be used within the
	/// rendering system but can't be private due to being used in make_shared.
	explicit material(material_manager& mgr, const material_description& description);
	/// Destroys the material and releases the underlying resources.
	~material();
	/// Forbids copy-construction of material.
	material(const material&) = delete;
	/// Forbids copy-construction of material.
	material& operator=(const material&) = delete;
	/// \brief Instructs the rendering system to run the handler function object when the material has
	/// completed loading and was transfered to the graphics device or to run the error_handler if an error
	/// occurred during loading or transfer.
	/**
	 * The handler function object must have the signature <code>void(const material_ptr& material)</code>.
	 * The error_handler function object must have the signature <code>void(std::exception_ptr)</code>. Both
	 * handlers are called on the thread calling this function or on a worker thread of the asset system. Both
	 * must fit into their respective handler function wrapper type material_completion_handler and
	 * error_handler.
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
	/// Checks if the material  is ready for use.
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
			throw path_not_found_exception("Error loading material '" + name_ + "'.");
	}
	/// Returns the current state of the material.
	state current_state() const noexcept {
		return current_state_;
	}
	/// Returns the name of the material.
	const std::string& name() const noexcept {
		return name_;
	}
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_HPP_ */
