/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_system.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/engine.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/rendering/renderer_system.hpp>

namespace mce {
namespace rendering {

renderer_system::renderer_system(asset::asset_manager& asset_mgr, graphics::texture_manager& tex_mgr,
								 model::model_data_manager& model_data_mgr, graphics::device& dev,
								 graphics::device_memory_manager_interface& mem_mgr,
								 graphics::destruction_queue_manager* destruction_manager,
								 graphics::transfer_manager& transfer_mgr)
		: mdl_mgr(model_data_mgr, dev, mem_mgr, destruction_manager, transfer_mgr),
		  mat_mgr(asset_mgr, tex_mgr) {}

renderer_system::renderer_system(core::engine& eng, graphics::graphics_system& gs)
		: mdl_mgr(eng.model_data_manager(), gs.device(), gs.memory_manager(),
				  &(gs.destruction_queue_manager()), gs.transfer_manager()),
		  mat_mgr(eng.asset_manager(), gs.texture_manager()) {}

renderer_system::~renderer_system() {}

} /* namespace rendering */
} /* namespace mce */
