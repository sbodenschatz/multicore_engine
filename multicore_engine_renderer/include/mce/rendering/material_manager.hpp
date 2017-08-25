/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/material_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MATERIAL_MANAGER_HPP_
#define MCE_RENDERING_MATERIAL_MANAGER_HPP_

namespace mce {
namespace graphics {
class texture_manager;
} // namespace graphics
namespace rendering {

class material_manager {
	graphics::texture_manager& tex_mgr;

	friend class material;

public:
	material_manager(graphics::texture_manager& tex_mgr) : tex_mgr{tex_mgr} {}
	~material_manager();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MATERIAL_MANAGER_HPP_ */
