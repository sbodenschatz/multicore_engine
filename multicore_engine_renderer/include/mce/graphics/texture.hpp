/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/texture.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_TEXTURE_HPP_
#define MCE_GRAPHICS_TEXTURE_HPP_

#include <atomic>
#include <boost/variant.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <mce/graphics/image.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class texture_manager;
class sampler;

class texture {
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
	boost::variant<boost::blank, image_2d, image_2d_layered> image_;
	boost::variant<boost::blank, image_view_2d, image_view_2d_layered> image_view_;

	void complete_loading(const asset::asset_ptr& texture_asset) noexcept;
	void complete_staging() noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;

	friend class texture_manager;

public:
	explicit texture(texture_manager& mgr, const std::string& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{name} {}
	explicit texture(texture_manager& mgr, std::string&& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{std::move(name)} {}
	~texture();
	texture(const texture&) = delete;
	texture& operator=(const texture&) = delete;
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler);
	bool ready() const noexcept {
		return current_state_ == state::ready;
	}
	bool has_error() const noexcept {
		return current_state_ == state::error;
	}
	void check_error_flag() const {
		if(current_state_ == state::error)
			throw path_not_found_exception("Error loading texture '" + name_ + "'.");
	}
	state current_state() const noexcept {
		return current_state_;
	}
	const std::string& name() const noexcept {
		return name_;
	}

	vk::DescriptorImageInfo bind(const sampler* sampler_to_use = nullptr) const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TEXTURE_HPP_ */
