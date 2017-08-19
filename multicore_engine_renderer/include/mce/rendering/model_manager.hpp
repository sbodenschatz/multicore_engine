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

namespace rendering {
class static_model;

class model_manager {
	model::model_data_manager& model_data_mgr_;
	std::shared_timed_mutex loaded_static_models_rw_lock_;
	boost::container::flat_map<std::string, std::shared_ptr<static_model>> loaded_static_models_;

public:
	model_manager(model::model_data_manager& model_data_mgr) : model_data_mgr_{model_data_mgr} {}
	~model_manager();

	template <typename F, typename E>
	static_model_ptr load_static_model(const std::string& name, F completion_handler, E error_handler);
	static_model_ptr load_static_model(const std::string& name);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_MODEL_MANAGER_HPP_ */
