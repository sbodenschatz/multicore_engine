/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/game_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_HPP_
#define CORE_GAME_STATE_HPP_

#include <memory>
#include <util/type_id.hpp>
#include <utility>
#include <vector>

namespace mce {
namespace core {
class system_state;
struct frame_time;

class game_state {
protected:
	std::vector<std::pair<util::type_id_t, std::unique_ptr<system_state>>> system_states_;

public:
	game_state() = default;
	virtual ~game_state();

	/**
	 * May only be called when no other threads are using the system_states collection.
	 * Usually this is called in initialization code only.
	 */
	template <typename T, typename... Args>
	T* add_system_state(Args&&... args) {
		system_states_.emplace_back(util::type_id<system_state>::id<T>(),
									std::make_unique<T>(std::forward<Args>(args)...));
		return system_states_.back().second.get();
	}

	template <typename T>
	T* get_system_state() const {
		auto tid = util::type_id<system_state>::id<T>();
		for(auto& sys : system_states_) {
			if(sys.first == tid) return sys.second.get();
		}
		return nullptr;
	}

	void process(const mce::core::frame_time& frame_time);
	void render(const mce::core::frame_time& frame_time);

	virtual void preprocess(const mce::core::frame_time& frame_time);
	virtual void postprocess(const mce::core::frame_time& frame_time);
	virtual void prerender(const mce::core::frame_time& frame_time);
	virtual void postrender(const mce::core::frame_time& frame_time);

	virtual void leave_pop();
	virtual void leave_push();
	virtual void reenter();
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_GAME_STATE_HPP_ */
