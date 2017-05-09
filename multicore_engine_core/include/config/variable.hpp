/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_HPP_
#define CONFIG_VARIABLE_HPP_

namespace mce {
namespace config {

class abstract_variable {};

template <typename T>
class variable_impl : public abstract_variable {};

} // namespace config
} // namespace mce

#endif /* CONFIG_VARIABLE_HPP_ */
