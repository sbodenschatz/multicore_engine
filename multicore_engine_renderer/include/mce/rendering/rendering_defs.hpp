/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/rendering_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERING_DEFS_HPP_
#define MCE_RENDERING_RENDERING_DEFS_HPP_

#include <mce/util/local_function.hpp>
#include <memory>

namespace mce {
namespace rendering {
class static_model;

using static_model_ptr = std::shared_ptr<const static_model>;
using static_model_completion_handler = util::local_function<128, void(const static_model_ptr& model)>;

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_RENDERING_DEFS_HPP_ */
