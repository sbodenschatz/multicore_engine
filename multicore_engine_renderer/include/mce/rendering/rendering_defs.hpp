/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/rendering_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERING_DEFS_HPP_
#define MCE_RENDERING_RENDERING_DEFS_HPP_

/**
 * \file
 * Provides typedefs for the rendering system.
 */

#include <mce/util/local_function.hpp>
#include <memory>
#include <string>

namespace mce {
namespace rendering {
class static_model;
class material_library;
class material;

/// Specifies the smart pointer type managing the lifetime of static_model objects.
using static_model_ptr = std::shared_ptr<const static_model>;
/// Specifies the function wrapper type used to wrap completion handlers for static_model loading.
using static_model_completion_handler = util::local_function<128, void(const static_model_ptr& model)>;

/// Specifies the smart pointer type managing the lifetime of material_library objects.
using material_library_ptr = std::shared_ptr<const material_library>;
/// Specifies the function wrapper type used to wrap completion handlers for material_library loading.
using material_library_completion_handler =
		util::local_function<128, void(const material_library_ptr& mat_lib)>;

/// Specifies the smart pointer type managing the lifetime of material objects.
using material_ptr = std::shared_ptr<const material>;
/// Specifies the function wrapper type used to wrap completion handlers for material loading.
using material_completion_handler = util::local_function<128, void(const material_ptr& mat)>;

/// Represents a description of a material as loaded from a material library.
struct material_description {
	std::string name;			 ///< The (unique) name of the defined material.
	std::string albedo_map_name; ///< The name of the texture used as the albedo map.
	std::string normal_map_name; ///< The name of the texture used as the normal map.
	/// The name of the texture defining the metallicness, roughness and ambient occlusion parameters.
	std::string material_map_name;
	std::string emission_map_name; ///< The name of the texture used as the emission map.
};

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_RENDERING_DEFS_HPP_ */
