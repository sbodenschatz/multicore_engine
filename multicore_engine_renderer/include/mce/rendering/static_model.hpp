/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/static_model.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_STATIC_MODEL_HPP_
#define MCE_RENDERING_STATIC_MODEL_HPP_

#include <atomic>
#include <mce/asset/asset_defs.hpp>
#include <mce/graphics/buffer.hpp>
#include <mce/model/model_defs.hpp>
#include <mce/model/model_format.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <mutex>
#include <vector>

namespace mce {
namespace rendering {
class model_manager;

class static_model : public std::enable_shared_from_this<static_model> {
public:
	/// Represents the status of static_model.
	enum class state { loading, staging, ready, error };

private:
	model_manager& mgr_;
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<static_model_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	model::static_model_meta_data meta_data_;
	model::polygon_model_ptr poly_model_;
	graphics::buffer vertex_index_buffer_;

	void complete_loading(const model::polygon_model_ptr& polygon_mdl) noexcept;
	void complete_staging() noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept;

	friend class model_manager;

public:
	explicit static_model(model_manager& mgr, const std::string& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{name} {}
	explicit static_model(model_manager& mgr, std::string&& name)
			: mgr_{mgr}, current_state_{state::loading}, name_{std::move(name)} {}
	~static_model();
	static_model(const static_model&) = delete;
	static_model& operator=(const static_model&) = delete;
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Static model '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			lock.unlock();
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Polygon model '" + name() + "' was cached as failed.")));
		} else {
			completion_handlers.emplace_back(std::move(handler));
			error_handlers.emplace_back(std::move(error_handler));
		}
	}
	bool ready() const noexcept;
	bool has_error() const noexcept;
	void check_error_flag() const;
	state current_state() const noexcept;
	const model::static_model_meta_data& meta_data() const noexcept;
	const std::string& name() const noexcept;
	void bind_vertices(vk::CommandBuffer cmd_buf);
	void bind_indices(vk::CommandBuffer cmd_buf, size_t mesh_index);
	void record_draw_call(vk::CommandBuffer cmd_buf, size_t mesh_index, uint32_t instances = 1);
	void draw_model_mesh(vk::CommandBuffer cmd_buf, size_t mesh_index, uint32_t instances = 1);
};
} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_STATIC_MODEL_HPP_ */
