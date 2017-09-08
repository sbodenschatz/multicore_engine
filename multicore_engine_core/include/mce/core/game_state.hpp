/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/game_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_HPP_
#define CORE_GAME_STATE_HPP_

#include <boost/any.hpp>
#include <mce/core/engine.hpp>
#include <mce/util/type_id.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace mce {
namespace core {
namespace detail {
template <typename State_Machine>
struct game_state_machine_policy;
} // namespace detail
class game_state_machine;
class system_state;
struct frame_time;

/// Represents the base class for game_states in the engine.
/**
 * The game_states are classes that implement logical states in the game, e.g. main menu, loading screen,
 * playing, pause.
 */
class game_state {
	std::vector<std::pair<util::type_id_t, std::unique_ptr<system_state>>> system_states_;
	mce::core::engine* engine_;
	mce::core::game_state_machine* state_machine_;
	mce::core::game_state* parent_state_;

	void process_leave_pop();
	void process_leave_push();
	void process_reenter(const boost::any& parameter);

	template <typename T>
	friend struct detail::game_state_machine_policy;

protected:
	/// Adds the system_state implemented by the class supplied in T to the game_state.
	/**
	 * An object of T is constructed using the given constructor arguments.
	 * This member function may only be called when no other threads are using the system_states collection.
	 * Usually it is called in initialization code only.
	 * The order of the calls to add_system_state determines the order in which the system_states are called
	 * within a frame.
	 * Usually only one object of a given type should be added because a second one could not be looked-up
	 * through get_system_state.
	 */
	template <typename T, typename... Args>
	T* add_system_state(Args&&... args) {
		auto sys = engine_->get_system<typename T::owner_system>();
		system_states_.emplace_back(util::type_id<system_state>::id<T>(),
									std::make_unique<T>(sys, std::forward<Args>(args)...));
		return static_cast<T*>(system_states_.back().second.get());
	}

public:
	/// Constructs the game_state with the given engine, state machine and parent state.
	game_state(mce::core::engine* engine, mce::core::game_state_machine* state_machine,
			   mce::core::game_state* parent_state);
	/// Enables polymorphic destruction for game_state.
	virtual ~game_state();

	/// \brief Looks up a system_state object of the given type and returns a pointer to it or nullptr if no
	/// such system_state exists.
	template <typename T>
	T* get_system_state() const {
		auto tid = util::type_id<system_state>::id<T>();
		for(auto& sys : system_states_) {
			if(sys.first == tid) return static_cast<T*>(sys.second.get());
		}
		return nullptr;
	}

	/// Implements the processing phase of the frame for the game_state.
	/**
	 * First calls preprocess.
	 * Then sytem_state::process is called on the system_states in the game_state in the order in
	 * which the
	 * system_states were added.
	 * Finally postprocess is called.
	 */
	void process(const mce::core::frame_time& frame_time);
	/// Implements the rendering phase of the frame for the game_state.
	/**
	 * First calls prerender.
	 * Then sytem_state::render is called on the system_states in the game_state in the order in which the
	 * system_states were added.
	 * Finally postrender is called.
	 */
	void render(const mce::core::frame_time& frame_time);

	/// Provides a hook for subclasses before system_state::process is called on system_states.
	virtual void preprocess(const mce::core::frame_time& frame_time);
	/// Provides a hook for subclasses after system_state::process is called on system_states.
	virtual void postprocess(const mce::core::frame_time& frame_time);
	/// Provides a hook for subclasses before system_state::render is called on system_states.
	virtual void prerender(const mce::core::frame_time& frame_time);
	/// Provides a hook for subclasses after system_state::render is called on system_states.
	virtual void postrender(const mce::core::frame_time& frame_time);

	/// Provides a hook for subclasses when the game_state is left because a new game_state was pushed on top.
	virtual void leave_pop();
	/// Provides a hook for subclasses when the game_state is left because it was popped.
	virtual void leave_push();

	/// \brief Provides a hook for subclasses when the game_state is reentered because the game_state on top
	/// of it was popped.
	/**
	 * Arbitrary parameter data can be passed using an any object. What parameter types are expected /
	 * accepted depends on the subclass.
	 */
	virtual void reenter(const boost::any& parameter);

	/// Allows access to the engine object associated with this state object.
	mce::core::engine* engine() const {
		return engine_;
	}

	/// Allows access to the parent state object.
	mce::core::game_state* parent_state() const {
		return parent_state_;
	}

	/// Allows access to the state machine in which this state object is used.
	mce::core::game_state_machine* state_machine() const {
		return state_machine_;
	}
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_GAME_STATE_HPP_ */
