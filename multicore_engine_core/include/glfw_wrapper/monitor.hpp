/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/monitor.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_MONITOR_HPP_
#define GLFW_WRAPPER_MONITOR_HPP_

#include <glfw_wrapper/glfw_defs.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct GLFWmonitor;
struct GLFWvidmode;

namespace mce {
namespace glfw_wrapper {
class instance;

/// Represents a monitor with associated config data reported by glfw.
/**
 * Because of glfw limitation all member functions may only be called on the main thread.
 */
class monitor {
	GLFWmonitor* monitor_;
	// cppcheck-suppress noExplicitConstructor
	monitor(GLFWmonitor* mon) : monitor_{mon} {}

	static video_mode convert_video_mode(const GLFWvidmode* m);

	friend class instance;
	friend class window;

public:
	/// Returns a monitor object representing the primary monitor.
	static monitor primary_monitor(const instance& inst);
	/// Returns a vector containing a monitor object for each connected monitor.
	static std::vector<monitor> monitors(const instance& inst);

	/// Returns a video_mode struct representing the current mode setting of the monitor.
	video_mode current_video_mode() const;
	/// Lists all video modes the represented monitor supports.
	std::vector<video_mode> video_modes() const;
	/// Returns the physical size of the monitor in millimeters.
	glm::ivec2 physical_size() const;
	/// Returns the virtual position of the monitor in virtual screen coordinates.
	/**
	 * See http://www.glfw.org/docs/latest/intro_guide.html#coordinate_systems for coordinate system
	 * explanation.
	 */
	glm::ivec2 virtual_position() const;
	/// Returns the name of the monitor device.
	std::string name() const;
	/// Returns the current gamma ramp of the monitor.
	glfw_wrapper::gamma_ramp gamma_ramp() const;
	/// Sets the monitors gamma ramp to the given value, should have 256 rgb points to be portable.
	void gamma_ramp(const glfw_wrapper::gamma_ramp& ramp);
	/// Sets the gamma ramp based on the given gamma exponent.
	void gamma(float gamma_exponent);
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_MONITOR_HPP_ */
