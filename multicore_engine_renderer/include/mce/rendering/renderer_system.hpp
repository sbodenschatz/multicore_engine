/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_SYSTEM_HPP_
#define MCE_RENDERING_RENDERER_SYSTEM_HPP_

#include <mce/core/system.hpp>

namespace mce {
namespace rendering {

class renderer_system : public core::system {
public:
	renderer_system();
	~renderer_system();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
