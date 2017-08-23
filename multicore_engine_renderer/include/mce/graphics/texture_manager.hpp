/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/texture_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_TEXTURE_MANAGER_HPP_
#define GRAPHICS_TEXTURE_MANAGER_HPP_

/**
 * Defines the texture_manager class.
 */

#include <boost/container/flat_map.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <mce/graphics/texture.hpp>
#include <memory>
#include <shared_mutex>
#include <string>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset
namespace graphics {
class device;
class device_memory_manager_interface;
class destruction_queue_manager;
class transfer_manager;

/// Manages the loading and lifetime of textures in the renderer.
class texture_manager {
	asset::asset_manager& asset_mgr_;
	graphics::device& dev_;
	graphics::device_memory_manager_interface& mem_mgr_;
	graphics::destruction_queue_manager* destruction_manager_;
	graphics::transfer_manager& transfer_mgr_;
	std::shared_timed_mutex loaded_textures_rw_lock_;
	boost::container::flat_map<std::string, std::shared_ptr<texture>> loaded_textures_;

	std::shared_ptr<texture> internal_load_texture(const std::string& name);

	friend class texture;

public:
	/// \brief Creates a texture_manager using the given underlying asset::asset_manager and the given
	/// required graphics subsystem resources.
	explicit texture_manager(asset::asset_manager& asset_mgr, graphics::device& dev,
							 graphics::device_memory_manager_interface& mem_mgr,
							 graphics::destruction_queue_manager* destruction_manager,
							 graphics::transfer_manager& transfer_mgr)
			: asset_mgr_{asset_mgr}, dev_{dev}, mem_mgr_{mem_mgr}, destruction_manager_{destruction_manager},
			  transfer_mgr_{transfer_mgr} {}
	/// Destroys the texture_manager and releases the underlying resources.
	~texture_manager();

	/// Forbids copying the texture_manager.
	texture_manager(const texture_manager&) = delete;
	/// Forbids copying the texture_manager.
	texture_manager& operator=(const texture_manager&) = delete;

	/// \brief Asynchronously loads the texture with the given name and calls the completion_handler
	/// function object on success or the error_handler function object on error.
	/**
	 * For the requirements for the function objects see texture::run_when_ready.
	 */
	template <typename F, typename E>
	texture_ptr load_texture(const std::string& name, F completion_handler, E error_handler) {
		auto tmp = internal_load_texture(name);
		tmp->run_when_ready(std::move(completion_handler), std::move(error_handler));
		return tmp;
	}
	/// Asynchronously loads the texture with the given name.
	texture_ptr load_texture(const std::string& name) {
		return internal_load_texture(name);
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_TEXTURE_MANAGER_HPP_ */
