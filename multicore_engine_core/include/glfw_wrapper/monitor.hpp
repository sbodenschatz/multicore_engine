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

class monitor {
	GLFWmonitor* monitor_;
	// cppcheck-suppress noExplicitConstructor
	monitor(GLFWmonitor* mon) : monitor_{mon} {}

	static video_mode convert_video_mode(const GLFWvidmode* m);

	friend class instance;
	friend class window;

public:
	static monitor primary_monitor(const instance& inst);
	static std::vector<monitor> monitors(const instance& inst);

	video_mode current_video_mode() const;
	std::vector<video_mode> video_modes() const;
	glm::ivec2 physical_size() const;
	glm::ivec2 virtual_position() const;
	std::string name() const;
	glfw_wrapper::gamma_ramp gamma_ramp() const;
	void gamma_ramp(const glfw_wrapper::gamma_ramp& ramp);
	void gamma(float gamma_exponent);
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_MONITOR_HPP_ */
