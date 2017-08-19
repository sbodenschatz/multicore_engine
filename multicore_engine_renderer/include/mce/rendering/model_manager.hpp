/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/model_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_MODEL_MANAGER_HPP_
#define MCE_RENDERING_MODEL_MANAGER_HPP_

#include <boost/container/flat_map.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <memory>
#include <shared_mutex>
#include <string>

namespace mce {
namespace model {
class model_data_manager;
} // namespace model

namespace graphics {
class device;
class device_memory_manager_interface;
class destruction_queue_manager;
class transfer_manager;
} // namespace graphics

namespace rendering {
class static_model;

class model_manager {
	model::model_data_manager& model_data_mgr_;
	graphics::device& dev_;
	graphics::device_memory_manager_interface& mem_mgr_;
	graphics::destruction_queue_manager* destruction_manager_;
	graphics::transfer_manager& transfer_mgr_;
	std::shared_timed_mutex loaded_static_models_rw_lock_;
	boost::container::flat_map<std::string, std::shared_ptr<static_model>> loaded_static_models_;

	std::shared_ptr<static_model> internal_load_static_model(const std::string& name);

	friend class static_model;

public:
	explicit model_manager(model::model_data_manager& model_data_mgr, graphics::device& dev,
						   graphics::device_memory_manager_interface& mem_mgr,
						   graphics::destruction_queue_manager* destruction_manager,
						   graphics::transfer_manager& transfer_mgr)
			: model_data_mgr_{model_data_mgr}, dev_{dev}, mem_mgr_{mem_mgr},
			  destruction_manager_{destruction_manager}, transfer_mgr_{transfer_mgr} {}
	~model_manager();

	model_manager(const model_manager&) = delete;
	model_manager& operator=(const model_manager&) = delete;

	template <typename F, typename E>
	static_model_ptr load_static_model(const std::string& name, F completion_handler, E error_handler);
	static_model_ptr load_static_model(const std::string& name);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MODEL_MANAGER_HPP_ */
