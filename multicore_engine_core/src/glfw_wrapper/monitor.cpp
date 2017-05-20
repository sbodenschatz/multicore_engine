/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/monitor.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <glfw_wrapper/monitor.hpp>
#include <iterator>

namespace mce {
namespace glfw_wrapper {

monitor monitor::primary_monitor(const instance&) {
	return glfwGetPrimaryMonitor();
}
std::vector<monitor> monitor::monitors(const instance&) {
	std::vector<monitor> res;
	int count = 0;
	auto m = glfwGetMonitors(&count);
	res.reserve(count);
	std::transform(m, m + count, std::back_inserter(res), [](auto m) { return monitor(m); });
	return res;
}

video_mode monitor::convert_video_mode(const GLFWvidmode* m) {
	video_mode res;
	res.width = m->width;
	res.height = m->height;
	res.red_bits = m->redBits;
	res.green_bits = m->greenBits;
	res.blue_bits = m->blueBits;
	res.refresh_rate = m->refreshRate;
	return res;
}

video_mode monitor::current_video_mode() const {
	return convert_video_mode(glfwGetVideoMode(monitor_));
}
std::vector<video_mode> monitor::video_modes() const {
	std::vector<video_mode> res;
	int count = 0;
	auto m = glfwGetVideoModes(monitor_, &count);
	res.reserve(count);
	std::transform(m, m + count, std::back_inserter(res), [](auto m) { return convert_video_mode(&m); });
	return res;
}
glm::ivec2 monitor::physical_size() const {
	glm::ivec2 res;
	glfwGetMonitorPhysicalSize(monitor_, &res.x, &res.y);
	return res;
}
glm::ivec2 monitor::virtual_position() const {
	glm::ivec2 res;
	glfwGetMonitorPos(monitor_, &res.x, &res.y);
	return res;
}
std::string monitor::name() const {
	return glfwGetMonitorName(monitor_);
}
glfw_wrapper::gamma_ramp monitor::gamma_ramp() const {
	glfw_wrapper::gamma_ramp res;
	auto ramp = glfwGetGammaRamp(monitor_);
	res.reserve(ramp->size);
	for(unsigned int i = 0; i < ramp->size; ++i) {
		res.emplace_back(ramp->red[i], ramp->green[i], ramp->blue[i]);
	}
	return res;
}
void monitor::gamma_ramp(const glfw_wrapper::gamma_ramp& ramp) {
	GLFWgammaramp tmp;
	std::vector<unsigned short> red(ramp.size());
	std::vector<unsigned short> green(ramp.size());
	std::vector<unsigned short> blue(ramp.size());
	tmp.size = static_cast<unsigned int>(ramp.size());
	for(unsigned int i = 0; i < ramp.size(); ++i) {
		red[i] = ramp[i].r;
		green[i] = ramp[i].g;
		blue[i] = ramp[i].b;
	}
	tmp.red = red.data();
	tmp.green = green.data();
	tmp.blue = blue.data();
	glfwSetGammaRamp(monitor_, &tmp);
}
void monitor::gamma(float gamma_exponent) {
	glfwSetGamma(monitor_, gamma_exponent);
}

} /* namespace glfw_wrapper */
} /* namespace mce */
