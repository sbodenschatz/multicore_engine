/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/model_defs.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_DEFS_HPP_
#define MODEL_MODEL_DEFS_HPP_

#include <memory>
#include <util/local_function.hpp>

namespace mce {
namespace model {

class polygon_model;
typedef std::shared_ptr<const polygon_model> polygon_model_ptr;
typedef util::local_function<128, void(const polygon_model_ptr& model)> polygon_model_completion_handler;

class collision_model;
typedef std::shared_ptr<const collision_model> collision_model_ptr;
typedef util::local_function<128, void(const collision_model_ptr& model)> collision_model_completion_handler;

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_DEFS_HPP_ */
