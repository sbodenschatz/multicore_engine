/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/windowing/window_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_WINDOWING_WINDOW_SYSTEM_HPP_
#define MCE_WINDOWING_WINDOW_SYSTEM_HPP_

/**
 * \file
 * Defines the window_system class.
 */

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
} // namespace core
namespace windowing {

/// Provides window functionality for the engine by opening a window and processing input from it.
class window_system : public core::system {
	core::engine& eng;
	std::string window_title_;
	glfw::instance instance_;
	std::unique_ptr<glfw::window> window_;

public:
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1000;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1000;
	}

	/// Creates the window_system taking the title for the window as a parameter.
	/**
	 * Should only be called in engine::add_system, but can't be made private and friended with engine
	 * because the construction takes place in std::make_unique.
	 *
	 * The properties of the window are controlled by the following configuration variables:
	 * - "display_mode" can be "windowed" meaning the engine uses a normal window, "windowed_fullscreen"
	 * meaning the window is displayed on a full monitor but using the monitors current video_mode, or
	 * "fullscreen" meaning the window is displayed using a specified video_mode on the specified monitor.
	 * Defaults to "windowed".
	 * - "resolution" specifies a 2d integer vector consisting of the desired width and height of the window.
	 * Works only in windowed display mode.
	 * - "monitor" specifies the index of the monitor to use for (windowed_)fullscreen mode.
	 * - "video_mode" specifies the index of the video mode of the used monitor to use in fullscreen mode.
	 */
	explicit window_system(core::engine& eng, const std::string& window_title);
	/// Destroys the window_system and releases the used resources.
	virtual ~window_system();

	/// Implements the hooked logic that happens at the beginning of the frame.
	/**
	 * Polls the inputs of the window and checks if the engine should stop.
	 */
	void preprocess(const mce::core::frame_time& frame_time) override;

	/// Allows access to the glfw::instance object.
	const glfw::instance& instance() const {
		return instance_;
	}

	/// Allows access to the glfw::instance object.
	glfw::instance& instance() {
		return instance_;
	}

	/// Allows access to the glfw::window object.
	const glfw::window& window() const {
		assert(window_);
		return *window_;
	}

	/// Allows access to the glfw::window object.
	glfw::window& window() {
		assert(window_);
		return *window_;
	}

	/// Returns the window title.
	const std::string& window_title() const {
		return window_title_;
	}
};

} /* namespace windowing */
} /* namespace mce */

#endif /* MCE_WINDOWING_WINDOW_SYSTEM_HPP_ */
