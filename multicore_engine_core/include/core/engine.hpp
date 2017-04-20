/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/engine.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef CORE_ENGINE_HPP_
#define CORE_ENGINE_HPP_

/**
 * \file
 * Defines the central management class for the engine.
 */

#include <atomic>
#include <cassert>
#include <memory>
#include <util/type_id.hpp>
#include <utility>
#include <vector>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset

namespace core {
class system;
class game_state_machine;
struct frame_time;

/// Represents the central management class for the subsystems of the engine.
class engine {
	std::atomic<bool> running_;
	std::unique_ptr<asset::asset_manager> asset_manager_;
	std::unique_ptr<mce::core::game_state_machine> game_state_machine_;
	std::vector<std::pair<util::type_id_t, std::unique_ptr<mce::core::system>>> systems_;

public:
	/// Constructs the engine.
	engine();
	~engine();

	void run();
	void process(const mce::core::frame_time& frame_time);
	void render(const mce::core::frame_time& frame_time);

	/**
	 * May only be called when no other threads are using the systems collection.
	 * Usually this is called in initialization code only.
	 */
	template <typename T, typename... Args>
	T* add_system(Args&&... args) {
		systems_.emplace_back(util::type_id<system>::id<T>(),
							  std::make_unique<T>(std::forward<Args>(args)...));
		return systems_.back().second.get();
	}

	template <typename T>
	T* get_system() const {
		auto tid = util::type_id<system>::id<T>();
		for(auto& sys : systems_) {
			if(sys.first == tid) return sys.second.get();
		}
		return nullptr;
	}

	/// Allows access to the asset_manager.
	const asset::asset_manager& asset_manager() const {
		assert(asset_manager_);
		return *asset_manager_;
	}
	/// Allows access to the asset_manager.
	asset::asset_manager& asset_manager() {
		assert(asset_manager_);
		return *asset_manager_;
	}

	/// Allows access to the game_state_machine.
	const mce::core::game_state_machine& game_state_machine() const {
		assert(game_state_machine_);
		return *game_state_machine_;
	}
	/// Allows access to the game_state_machine.
	mce::core::game_state_machine& game_state_machine() {
		assert(game_state_machine_);
		return *game_state_machine_;
	}

	/// \brief Returns a bool indicating if the engine is flagged as running (false if the engine is marked to
	/// stop and tear down at the next opportunity).
	bool running() const {
		return running_;
	}

	/// Marks the engine to stop at the next opportunity (usually the next frame).
	void stop() {
		running_ = false;
	}
};

} // namespace core
} // namespace mce

#endif /* CORE_ENGINE_HPP_ */
