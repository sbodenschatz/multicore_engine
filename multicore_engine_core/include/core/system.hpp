/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_SYSTEM_HPP_
#define CORE_SYSTEM_HPP_

namespace mce {
namespace core {
struct frame_time;

class system {
public:
	system();
	virtual ~system();
	virtual void preprocess(const mce::core::frame_time& frame_time);
	virtual void postprocess(const mce::core::frame_time& frame_time);
	virtual void prerender(const mce::core::frame_time& frame_time);
	virtual void postrender(const mce::core::frame_time& frame_time);
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_SYSTEM_HPP_ */
