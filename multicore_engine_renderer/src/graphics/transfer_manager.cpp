/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/transfer_manager.hpp>

namespace mce {
namespace graphics {

transfer_manager::transfer_manager(device& dev, device_memory_manager_interface& mm,
								   destruction_queue_manager* dqm)
		: dev{dev}, mm{mm}, dqm{dqm}, transfer_cmd_pool{dev, dev.transfer_queue_index().first, true, true},
		  ownership_cmd_pool{dev, dev.graphics_queue_index().first, true, true},
		  staging_buffer{dev, mm, dqm, 1 << 27, vk::BufferUsageFlagBits::eTransferSrc},
		  chunk_placer{staging_buffer.mapped_pointer(), staging_buffer.size()} {}

transfer_manager::~transfer_manager() {
	if(!dqm) dev.native_device().waitIdle();
}

} /* namespace graphics */
} /* namespace mce */
