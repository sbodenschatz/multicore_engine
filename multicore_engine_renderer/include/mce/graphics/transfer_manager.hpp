/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_TRANSFER_MANAGER_HPP_
#define MCE_GRAPHICS_TRANSFER_MANAGER_HPP_

#include <mce/graphics/buffer.hpp>
#include <mce/graphics/command_pool.hpp>
#include <vector>

namespace mce {
namespace graphics {

class transfer_manager {
private:
	struct transfer_job {};

	device& dev;
	device_memory_manager_interface& mm;
	destruction_queue_manager* dqm;
	std::vector<transfer_job> waiting_jobs;
	std::vector<std::vector<transfer_job>> running_jobs;
	command_pool transfer_cmd_pool;
	command_pool ownership_cmd_pool;
	std::vector<vk::UniqueCommandBuffer> transfer_command_bufers;
	std::vector<vk::UniqueCommandBuffer> pending_ownership_command_buffers;
	buffer staging_buffer;

public:
	transfer_manager(device& dev, device_memory_manager_interface& mm, destruction_queue_manager* dqm);
	~transfer_manager();
	// TODO Handle release of ring buffer slots when the buffer was replaced while the transfer was executing.
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TRANSFER_MANAGER_HPP_ */
