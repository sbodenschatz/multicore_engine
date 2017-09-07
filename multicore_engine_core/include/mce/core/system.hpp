/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_SYSTEM_HPP_
#define CORE_SYSTEM_HPP_

namespace mce {
namespace core {
struct frame_time;

/// Provides the base class for systems providing functionality in the engine.
class system {
public:
	/// Constructs a system.
	system() = default;
	/// Enables polymorphic destruction for systems.
	virtual ~system() = default;
	/// Hook function called at the start of the processing phase of the frame.
	virtual void preprocess(const mce::core::frame_time& frame_time);
	/// Hook function called at the end of the processing phase of the frame.
	virtual void postprocess(const mce::core::frame_time& frame_time);
	/// Hook function called at the start of the rendering phase of the frame.
	virtual void prerender(const mce::core::frame_time& frame_time);
	/// Hook function called at the end of the rendering phase of the frame.
	virtual void postrender(const mce::core::frame_time& frame_time);
	/// \brief Is called when the system is added to the engine to determine the order in which the preprocess
	/// and prerender hooks of the system should be called in relation to other systems.
	virtual int pre_phase_ordering() const noexcept = 0;
	/// \brief Is called when the system is added to the engine to determine the order in which the
	/// postprocess and postrender hooks of the system should be called in relation to other systems.
	virtual int post_phase_ordering() const noexcept = 0;
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_SYSTEM_HPP_ */
