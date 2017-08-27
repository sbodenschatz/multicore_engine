/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_MANAGER_HPP_
#define MCE_RENDERING_MATERIAL_MANAGER_HPP_

#include <boost/container/flat_map.hpp>
#include <mce/rendering/material.hpp>
#include <mce/rendering/material_library.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <memory>
#include <shared_mutex>
#include <string>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset
namespace graphics {
class texture_manager;
} // namespace graphics
namespace rendering {
namespace detail {
struct material_manager_dependencies {
	asset::asset_manager& amgr;
	graphics::texture_manager& tex_mgr;
};
} // namespace detail

class material_manager {
	std::shared_ptr<const detail::material_manager_dependencies> dependencies_;
	std::shared_timed_mutex rw_lock_;
	boost::container::flat_map<std::string, std::shared_ptr<material>> loaded_materials_;
	boost::container::flat_map<std::string, std::shared_ptr<material_library>> loaded_material_libs_;

	std::shared_ptr<material> internal_load_material(const std::string& name);
	std::shared_ptr<material_library> internal_load_material_lib(const std::string& name);

	friend class material;

public:
	explicit material_manager(asset::asset_manager& asset_mgr, graphics::texture_manager& tex_mgr)
			: dependencies_{std::make_shared<detail::material_manager_dependencies>(
					  detail::material_manager_dependencies{asset_mgr, tex_mgr})} {}
	~material_manager();

	/// Forbids copying the material_manager.
	material_manager(const material_manager&) = delete;
	/// Forbids copying the material_manager.
	material_manager& operator=(const material_manager&) = delete;

	/// \brief Asynchronously loads the material with the given name and calls the completion_handler
	/// function object on success or the error_handler function object on error.
	/**
	 * For the requirements for the function objects see material::run_when_ready.
	 */
	template <typename F, typename E>
	material_ptr load_material(const std::string& name, F completion_handler, E error_handler) {
		auto tmp = internal_load_material(name);
		tmp->run_when_ready(std::move(completion_handler), std::move(error_handler));
		return tmp;
	}
	/// Asynchronously loads the material with the given name.
	material_ptr load_material(const std::string& name) {
		return internal_load_material(name);
	}
	/// \brief Asynchronously loads the material library with the given name and calls the completion_handler
	/// function object on success or the error_handler function object on error.
	/**
	 * For the requirements for the function objects see material_library::run_when_ready.
	 */
	template <typename F, typename E>
	material_library_ptr load_material_library(const std::string& name, F completion_handler,
											   E error_handler) {
		auto tmp = internal_load_material_lib(name);
		tmp->run_when_ready(std::move(completion_handler), std::move(error_handler));
		return tmp;
	}
	/// Asynchronously loads the material library with the given name.
	material_library_ptr load_material_library(const std::string& name) {
		return internal_load_material_lib(name);
	}
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_MANAGER_HPP_ */
