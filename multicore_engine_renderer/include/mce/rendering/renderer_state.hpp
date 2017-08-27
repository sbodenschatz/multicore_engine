/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_STATE_HPP_
#define MCE_RENDERING_RENDERER_STATE_HPP_

#include <mce/core/system_state.hpp>

namespace mce {
namespace rendering {

class renderer_state : public core::system_state {
public:
	explicit renderer_state(core::system* sys);
	~renderer_state();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_STATE_HPP_ */
