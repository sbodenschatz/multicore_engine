/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_SYSTEM_HPP_
#define MCE_RENDERING_RENDERER_SYSTEM_HPP_

#include <mce/core/system.hpp>
#include <mce/rendering/material_manager.hpp>
#include <mce/rendering/model_manager.hpp>

namespace mce {
namespace rendering {

class renderer_system : public core::system {

	model_manager mdl_mgr;
	material_manager mat_mgr;

	friend class static_model_component;

public:
	renderer_system(asset::asset_manager& asset_mgr, graphics::texture_manager& tex_mgr,
					model::model_data_manager& model_data_mgr, graphics::device& dev,
					graphics::device_memory_manager_interface& mem_mgr,
					graphics::destruction_queue_manager* destruction_manager,
					graphics::transfer_manager& transfer_mgr);
	~renderer_system();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
