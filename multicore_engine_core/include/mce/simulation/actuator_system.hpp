/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_
#define MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_

/**
 * \file
 * Defines the actuator_system class.
 */

#include <boost/container/flat_map.hpp>
#include <mce/core/system.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/simulation/simulation_defs.hpp>
#include <mce/util/local_function.hpp>
#include <mutex>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace simulation {

/// Manages movement patterns that can be applied to entities using actuator_state and actuator_component.
/**
 * The movement patterns are defined by function objects that are callable using the signature
 * <code>void(const mce::core::frame_time&, entity::entity&)</code>.
 * These function objects are called once per frame and can manipulate the entity object given to them to
 * create the desired movement. The registered function objects are copied into the actuator_component objects
 * to provide each instance with a separate local state. The function objects on different actuator_component
 * objects are called in parallel and are not synchronized by the actuator_system and must therefore only
 * manipulate their owner entity and local state and read read-only objects or ensure synchronization for
 * other accesses.
 */
class actuator_system : public core::system {
	core::engine& eng_;
	boost::container::flat_map<std::string, movement_pattern_function> movement_patterns_;
	mutable std::mutex mtx;

public:
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1200;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1200;
	}

	/// Creates an actuator_system for the given engine objects.
	explicit actuator_system(core::engine& eng);
	/// Destroys the actuator_system.
	~actuator_system();

	/// \brief Sets the movement_pattern_function object for the given movement pattern name, replacing the
	/// previous value if it was already set.
	void set_movement_pattern(const std::string& name, const movement_pattern_function& pattern_function) {
		std::lock_guard<std::mutex> lock(mtx);
		movement_patterns_[name] = pattern_function;
	}

	/// \brief Looks up the movement_pattern_function with the given name, throwing a std::out_of_range
	/// exception if no object was set for the given name.
	movement_pattern_function find_movement_pattern(const std::string& name) const {
		std::lock_guard<std::mutex> lock(mtx);
		return movement_patterns_.at(name);
	}

	/// Removes the mapped movement_pattern_function object for the given name.
	void remove_movement_pattern(const std::string& name) {
		std::lock_guard<std::mutex> lock(mtx);
		movement_patterns_.erase(name);
	}
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_ */
