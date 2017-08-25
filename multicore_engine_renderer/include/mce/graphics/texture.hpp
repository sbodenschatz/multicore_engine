/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/texture.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_TEXTURE_HPP_
#define MCE_GRAPHICS_TEXTURE_HPP_

/**
 * Defines the texture class.
 */

#include <atomic>
#include <boost/variant.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <mce/graphics/image.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace gli {
class texture;
} // namespace gli

namespace mce {
namespace graphics {
class texture_manager;
class sampler;
class base_image;

/// Represents a texture image asset.
/**
 * Textures are a combination of image and image_view loaded from the asset system.
 */
class texture : public std::enable_shared_from_this<texture> {
public:
	/// Represents the status of texture.
	enum class state { loading, staging, ready, error };

private:
	texture_manager& mgr_;
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<texture_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	boost::variant<boost::blank, image_1d, image_1d_layered, image_2d, image_2d_layered, image_3d, image_cube,
				   image_cube_layered>
			image_;
	boost::variant<boost::blank, image_view_1d, image_view_1d_layered, image_view_2d, image_view_2d_layered,
				   image_view_3d, image_view_cube, image_view_cube_layered>
			image_view_;
	vk::Image img_handle;
	vk::ImageView img_view_handle;

	void complete_loading(const asset::asset_ptr& texture_asset) noexcept;
	void complete_staging() noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;

	// Intentionally only declared but not implemented here despite being a template function because it is
	// only used in a single compilation unit.
	template <typename T>
	base_image* create_image(vk::Format format, const gli::texture& tex);
	// Intentionally only declared but not implemented here despite being a template function because it is
	// only used in a single compilation unit.
	template <typename T>
	base_image* create_image_layered(vk::Format format, const gli::texture& tex);

	friend class texture_manager;

public:
	/// \brief Creates an texture object with the given name. Should only be used within the rendering system
	/// but can't be private due to being used in make_shared.
	explicit texture(texture_manager& mgr, const std::string& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{name} {}
	/// \brief Creates an texture object with the given name. Should only be used within the rendering system
	/// but can't be private due to being used in make_shared.
	explicit texture(texture_manager& mgr, std::string&& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{std::move(name)} {}
	/// Destroys the texture and releases the underlying resources.
	~texture();
	/// Forbids copy-construction of texture objects.
	texture(const texture&) = delete;
	/// Forbids copy-construction of texture objects.
	texture& operator=(const texture&) = delete;
	/// \brief Instructs the rendering system to run the handler function object when the texture has
	/// completed loading and was transfered to the graphics device or to run the error_handler if an error
	/// occurred during loading or transfer.
	/**
	 * The handler function object must have the signature <code>void(const texture_ptr& texture)</code>.
	 * The error_handler function object must have the signature <code>void(std::exception_ptr)</code>.
	 * Both handlers are called on the thread calling this function, on the thread running
	 * transfer_manager::start_frame or on a worker thread of the asset system. Both must fit into their
	 * respective handler function wrapper type texture_completion_handler and error_handler.
	 */
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Texture '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			lock.unlock();
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Texture '" + name() + "' was cached as failed.")));
		} else {
			completion_handlers.emplace_back(std::move(handler));
			error_handlers.emplace_back(std::move(error_handler));
		}
	}
	/// Checks if the texture is ready for use.
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
			throw path_not_found_exception("Error loading texture '" + name_ + "'.");
	}
	/// Returns the current state of the texture.
	state current_state() const noexcept {
		return current_state_;
	}
	/// Returns the name of the texture.
	const std::string& name() const noexcept {
		return name_;
	}

	/// \brief Returns a DescriptorImageInfo that can be used to bind the texture in a descriptor set
	/// optionally using a given sampler.
	/**
	 * Requires the object to be ready for use. Calling this member function on a non-ready object results in
	 * undefined behavior due to a race condition.
	 */
	vk::DescriptorImageInfo bind(const sampler* sampler_to_use = nullptr) const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TEXTURE_HPP_ */
