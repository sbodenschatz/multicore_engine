/*
 * Multi-Core Engine project
 * File /multicore_engine_graphics_test/include/mce/graphics/graphics_test.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_TEST_HPP_
#define MCE_GRAPHICS_GRAPHICS_TEST_HPP_

#include <boost/filesystem.hpp>
#include <chrono>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <mce/asset/asset_manager.hpp>
#include <mce/containers/dynamic_array.hpp>
#include <mce/glfw/instance.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/command_pool.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/simple_uniform_buffer.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <mce/graphics/window.hpp>
#include <mce/model/model_data_manager.hpp>
#include <mce/rendering/model_manager.hpp>

namespace mce {
namespace graphics {
class framebuffer;

class graphics_test {
	asset::asset_manager amgr_;
	model::model_data_manager mdmgr;
	glfw::instance glfw_inst_;
	glfw::window glfw_win_;
	instance inst_;
	device dev_;
	window win_;
	device_memory_manager mem_mgr_;
	command_pool render_cmd_pool_;
	destruction_queue_manager dqm_;
	transfer_manager tmgr_;
	rendering::model_manager mmgr;
	graphics_manager gmgr_;
	vk::UniqueSemaphore tmp_semaphore_;
	containers::dynamic_array<vk::UniqueSemaphore> acquire_semaphores_;
	containers::dynamic_array<vk::UniqueSemaphore> present_semaphores_;
	containers::dynamic_array<vk::UniqueFence> fences_;
	containers::dynamic_array<simple_uniform_buffer> uniform_buffers_;
	buffer vertex_buffer_;
	bool vb_ready_ = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_t_;
	std::shared_ptr<const framebuffer_config> fbcfg_;
	std::shared_ptr<const pipeline_layout> pll_;
	std::shared_ptr<const subpass_graph> spg_;
	std::shared_ptr<const render_pass> rp_;
	std::shared_ptr<const shader_module> vert_shader_;
	std::shared_ptr<const shader_module> frag_shader_;
	std::shared_ptr<const pipeline_config> plc_;
	std::shared_ptr<const pipeline> pl_;
	std::unique_ptr<framebuffer> fb_;
	rendering::static_model_ptr mdl_;

	struct vertex {
		glm::vec3 pos;
		glm::vec3 color;
	};

	struct uniform_data {
		glm::mat4 projection;
		glm::mat4 model;
	};

public:
	graphics_test();
	~graphics_test();
	void run();

	static boost::filesystem::path exe_path;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_TEST_HPP_ */
