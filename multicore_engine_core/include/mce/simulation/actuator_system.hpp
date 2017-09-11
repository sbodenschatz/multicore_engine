/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_
#define MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_

#include <boost/container/flat_map.hpp>
#include <mce/core/system.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/util/local_function.hpp>
#include <mutex>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace simulation {

class actuator_system : public core::system {
public:
	using movement_pattern_function = util::local_function<256, void(entity::entity&)>;

private:
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

	explicit actuator_system(core::engine& eng);
	~actuator_system();

	void set_movement_pattern(const std::string& name, const movement_pattern_function& pattern_function) {
		std::lock_guard<std::mutex> lock(mtx);
		movement_patterns_[name] = pattern_function;
	}

	movement_pattern_function find_movement_pattern(const std::string& name) const {
		std::lock_guard<std::mutex> lock(mtx);
		return movement_patterns_.at(name);
	}

	void remove_movement_pattern(const std::string& name) {
		std::lock_guard<std::mutex> lock(mtx);
		movement_patterns_.erase(name);
	}
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_ */
