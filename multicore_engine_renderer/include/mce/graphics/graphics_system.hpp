/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/graphics_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_
#define MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_

#include <mce/core/system.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace core {
class engine;
class window_system;
} // namespace core
namespace graphics {

class graphics_system : public core::system {
	core::engine& eng;
	graphics::instance instance_;
	graphics::device device_;
	graphics::window window_;
	graphics::device_memory_manager memory_manager_;
	graphics::destruction_queue_manager destruction_queue_manager_;
	graphics::transfer_manager transfer_manager_;
	graphics::texture_manager texture_manager_;
	graphics::graphics_manager graphics_manager_;

public:
	graphics_system(core::engine& eng, core::window_system& win_sys,
					const std::vector<std::string>& extensions = {},
					unsigned int validation_level = instance::default_validation_level);
	virtual ~graphics_system();

	const graphics::destruction_queue_manager& destruction_queue_manager() const {
		return destruction_queue_manager_;
	}

	graphics::destruction_queue_manager& destruction_queue_manager() {
		return destruction_queue_manager_;
	}

	const graphics::device& device() const {
		return device_;
	}

	graphics::device& device() {
		return device_;
	}

	const graphics::graphics_manager& graphics_manager() const {
		return graphics_manager_;
	}

	graphics::graphics_manager& graphics_manager() {
		return graphics_manager_;
	}

	const graphics::instance& instance() const {
		return instance_;
	}

	graphics::instance& instance() {
		return instance_;
	}

	const graphics::device_memory_manager& memory_manager() const {
		return memory_manager_;
	}

	graphics::device_memory_manager& memory_manager() {
		return memory_manager_;
	}

	const graphics::texture_manager& texture_manager() const {
		return texture_manager_;
	}

	graphics::texture_manager& texture_manager() {
		return texture_manager_;
	}

	const graphics::transfer_manager& transfer_manager() const {
		return transfer_manager_;
	}

	graphics::transfer_manager& transfer_manager() {
		return transfer_manager_;
	}

	const graphics::window& window() const {
		return window_;
	}

	graphics::window& window() {
		return window_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_ */
