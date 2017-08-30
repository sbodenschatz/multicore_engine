/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/window_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CORE_WINDOW_SYSTEM_HPP_
#define MCE_CORE_WINDOW_SYSTEM_HPP_

#include <cassert>
#include <mce/core/system.hpp>
#include <mce/glfw/instance.hpp>
#include <memory>

namespace mce {
namespace glfw {
class window;
} // namespace glfw
namespace core {
class engine;

class window_system : public system {
	engine& eng;
	glfw::instance instance_;
	std::unique_ptr<glfw::window> window_;

public:
	explicit window_system(engine& eng);
	virtual ~window_system();

	void preprocess(const mce::core::frame_time& frame_time) override;

	const glfw::instance& instance() const {
		return instance_;
	}

	glfw::instance& instance() {
		return instance_;
	}

	const glfw::window& window() const {
		assert(window_);
		return *window_;
	}

	glfw::window& window() {
		assert(window_);
		return *window_;
	}
};

} /* namespace core */
} /* namespace mce */

#endif /* MCE_CORE_WINDOW_SYSTEM_HPP_ */
