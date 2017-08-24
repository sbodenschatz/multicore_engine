/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_HPP_
#define MCE_RENDERING_MATERIAL_HPP_

#include <mce/graphics/graphics_defs.hpp>

namespace mce {
namespace rendering {

class material {
	graphics::texture_ptr albedo_map_;
	graphics::texture_ptr normal_map_;
	graphics::texture_ptr material_param_map_; // Metallic,Roughness,Ambient occlusion
	graphics::texture_ptr emission_map_;

public:
	material();
	~material();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_HPP_ */
