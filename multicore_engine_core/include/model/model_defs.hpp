/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/model_defs.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_DEFS_HPP_
#define MODEL_MODEL_DEFS_HPP_

#include <memory>
#include <util/local_function.hpp>

namespace mce {
namespace model {

class polygon_model;
/// Specifies the smart pointer type managing the lifetime of polygon_model objects.
typedef std::shared_ptr<const polygon_model> polygon_model_ptr;
/// Specifies the function wrapper type used to wrap completion handlers for polygon_model loading.
typedef util::local_function<128, void(const polygon_model_ptr& model)> polygon_model_completion_handler;

class collision_model;
/// Specifies the smart pointer type managing the lifetime of collision_model objects.
typedef std::shared_ptr<const collision_model> collision_model_ptr;
/// Specifies the function wrapper type used to wrap completion handlers for collision_model loading.
typedef util::local_function<128, void(const collision_model_ptr& model)> collision_model_completion_handler;

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_DEFS_HPP_ */
