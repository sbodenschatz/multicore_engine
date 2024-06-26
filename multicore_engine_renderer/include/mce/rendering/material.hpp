/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_HPP_
#define MCE_RENDERING_MATERIAL_HPP_

/**
 * \file
 * Defines the material class of the rendering system.
 */

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
namespace graphics {
class texture_manager;
class descriptor_set;
} // namespace graphics
namespace rendering {

/// \brief Describes the appearance properties of surfaces of a model mesh represented by multiple texture
/// maps describing the values of the parameters varying across the surface.
class material : public std::enable_shared_from_this<material> {
public:
	/// Represents the status of a material.
	enum class state { initial, loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::atomic<size_t> pending_lib_load_count;
	std::vector<material_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	graphics::texture_ptr albedo_map_;
	graphics::texture_ptr normal_map_;
	graphics::texture_ptr material_map_;
	graphics::texture_ptr emission_map_;

	void texture_loaded(const graphics::texture_ptr& tex) noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;
	void try_raise_error_flag(std::exception_ptr e) noexcept;

	bool try_start_loading(graphics::texture_manager& mgr, const material_description& description) noexcept;
	void check_load_fails();
	void process_pending_material_loads(graphics::texture_manager& tex_mgr, const material_library_ptr& lib);

	friend class material_manager;

public:
	/// \brief Creates an material object with the given name. Should only be used within the
	/// rendering system but can't be private due to being used in make_shared.
	explicit material(const std::string& name)
			: current_state_{state::initial}, name_{name}, pending_lib_load_count{0} {}
	/// \brief Creates an material object with the given name. Should only be used within the
	/// rendering system but can't be private due to being used in make_shared.
	explicit material(std::string&& name)
			: current_state_{state::initial}, name_{name}, pending_lib_load_count{0} {}
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

	/// Returns the texture map describing the albedo parameter (color of the surface) of the material.
	/**
	 * Requires the object to be ready for use. Calling this member function on a non-ready object results in
	 * undefined behavior due to a race condition.
	 */
	const graphics::texture_ptr& albedo_map() const noexcept {
		return albedo_map_;
	}

	/// Returns the texture map describing the emission parameter (emitted light) of the material.
	/**
	 * Requires the object to be ready for use. Calling this member function on a non-ready object results in
	 * undefined behavior due to a race condition.
	 */
	const graphics::texture_ptr& emission_map() const noexcept {
		return emission_map_;
	}

	/// \brief Returns the texture map describing the metallicness (0=dielectric, 1=metallic), roughness
	/// (0=smooth, 1=rough) and ambient occlusion (multiplier for occlusions in the surface geometry)
	/// parameters of the material.
	/**
	 * Requires the object to be ready for use. Calling this member function on a non-ready object results in
	 * undefined behavior due to a race condition.
	 */
	const graphics::texture_ptr& material_map() const noexcept {
		return material_map_;
	}

	/// Returns the texture map describing the surface normals of the material using the DXT5nm compression.
	/**
	 * Requires the object to be ready for use. Calling this member function on a non-ready object results in
	 * undefined behavior due to a race condition.
	 */
	const graphics::texture_ptr& normal_map() const noexcept {
		return normal_map_;
	}

	/// Updates the given descriptor_set to contain the data for the material.
	void bind(graphics::descriptor_set& ds) const;
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_HPP_ */
