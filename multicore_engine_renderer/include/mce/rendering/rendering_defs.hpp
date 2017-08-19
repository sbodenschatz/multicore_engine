/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/rendering_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERING_DEFS_HPP_
#define MCE_RENDERING_RENDERING_DEFS_HPP_

/**
 * \file
 * Provides typedefs for the rendering system.
 */

#include <mce/util/local_function.hpp>
#include <memory>

namespace mce {
namespace rendering {
class static_model;

/// Specifies the smart pointer type managing the lifetime of static_model objects.
using static_model_ptr = std::shared_ptr<const static_model>;
/// Specifies the function wrapper type used to wrap completion handlers for static_model loading.
using static_model_completion_handler = util::local_function<128, void(const static_model_ptr& model)>;

} // namespace rendering
} // namespace mce

#endif /* MCE_RENDERING_RENDERING_DEFS_HPP_ */
