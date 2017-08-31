/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/engine.hpp
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
#include <mce/core/system.hpp>
#include <mce/core/version.hpp>
#include <mce/util/type_id.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset

namespace model {
class model_data_manager;
} // namespace model

namespace config {
class config_store;
} // namespace config

namespace core {
class system;
class game_state_machine;
struct frame_time;

/// Represents the central management class for the subsystems of the engine.
class engine {
	std::atomic<bool> running_;
	software_metadata engine_metadata_;
	software_metadata application_metadata_;
	std::unique_ptr<asset::asset_manager> asset_manager_;
	std::unique_ptr<config::config_store> config_store_;
	std::unique_ptr<model::model_data_manager> model_data_manager_;
	std::vector<std::pair<util::type_id_t, std::unique_ptr<mce::core::system>>> systems_;
	std::vector<std::pair<int, mce::core::system*>> systems_pre_phase_ordered;
	std::vector<std::pair<int, mce::core::system*>> systems_post_phase_ordered;
	std::unique_ptr<mce::core::game_state_machine> game_state_machine_;

	void refresh_system_ordering();
	void initialize_config();

public:
	/// Constructs the engine.
	engine();
	/// Destroys the engine.
	~engine();

	/// Enters the core processing loop of the engine and returns only after the engine is stopped.
	void run();
	/// Runs the processing phase of a frame where logic and simulation is handled.
	void process(const mce::core::frame_time& frame_time);
	/// Runs the rendering phase of a frame where the graphics output is generated.
	void render(const mce::core::frame_time& frame_time);

	/// Adds the system implemented by the class supplied in T to the engine.
	/**
	 * An object of T is constructed using the given constructor arguments prepended with a reference to the
	 * engine object.
	 * This member function may only be called when no other threads are using the systems collection.
	 * Usually it is called in initialization code only.
	 * Usually only one object of a given type should be added because a second one could not be looked-up
	 * through get_system.
	 *
	 * For the phases of a frame (process and render) the member functions system::preprocess or
	 * system::prerender are called before calling the game_state::process or game_state::render of the
	 * current game_state. The systems are called sorted by the results of their pre_phase_ordering virtual
	 * member functions.
	 * After the the game_state::process or game_state::render of the current game_state the member functions
	 * system::postprocess or system::postrender are called sorted by the results of their post_phase_ordering
	 * virtual member functions in descending order.
	 */
	template <typename T, typename... Args>
	T* add_system(Args&&... args) {
		systems_.emplace_back(util::type_id<system>::id<T>(),
							  std::make_unique<T>(*this, std::forward<Args>(args)...));
		auto sys = systems_.back().second.get();
		systems_pre_phase_ordered.emplace_back(sys->pre_phase_ordering(), sys);
		systems_post_phase_ordered.emplace_back(sys->post_phase_ordering(), sys);
		refresh_system_ordering();
		return static_cast<T*>(sys);
	}

	/// \brief Looks up a system object of the given type and returns a pointer to it or nullptr if no such
	/// system exists.
	template <typename T>
	T* get_system() const {
		auto tid = util::type_id<system>::id<T>();
		for(auto& sys : systems_) {
			if(sys.first == tid) return static_cast<T*>(sys.second.get());
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

	/// Allows access to the model_data_manager.
	const model::model_data_manager& model_data_manager() const {
		assert(model_data_manager_);
		return *model_data_manager_;
	}

	/// Allows access to the model_data_manager.
	model::model_data_manager& model_data_manager() {
		assert(model_data_manager_);
		return *model_data_manager_;
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

	/// Marks the engine as running.
	void set_running() {
		running_ = true;
	}

	/// Allows access to the config_store.
	const config::config_store& config_store() const {
		assert(config_store_);
		return *config_store_;
	}

	/// Allows access to the config_store.
	config::config_store& config_store() {
		assert(config_store_);
		return *config_store_;
	}

	/// Allows read-only access to the metadata for the application using this engine object.
	const software_metadata& application_metadata() const {
		return application_metadata_;
	}

	/// Allows read-write access to the metadata for the application using this engine object.
	software_metadata& application_metadata() {
		return application_metadata_;
	}

	/// Sets the metadata for the application using this engine object.
	void application_metadata(const software_metadata& application_metadata) {
		application_metadata_ = application_metadata;
	}

	/// Allows read-only access to the metadata for the engine.
	const software_metadata& engine_metadata() const {
		return engine_metadata_;
	}
};

} // namespace core
} // namespace mce

#endif /* CORE_ENGINE_HPP_ */
