/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/glfw/name_mapping.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GLFW_NAME_MAPPING_HPP_
#define MCE_GLFW_NAME_MAPPING_HPP_

/**
 * \file
 * Defines functions to map keys and mouse buttons to strings and vice-versa.
 */

#include <boost/optional.hpp>
#include <mce/glfw/glfw_defs.hpp>
#include <string>

namespace mce {
namespace glfw {

/// Returns a string representation for the given key.
const std::string& to_string(key k);
/// Returns the key represented by the given name or key::unknown if the name doesn't map to a valid key.
key key_from_string(const std::string& name);
/// Returns a vector listing all known keys.
const std::vector<key>& all_keys();

/// Returns a string representation for the given mouse_button.
const std::string& to_string(mouse_button b);
/// Returns the mouse_button represented by the given name or an empty optional if the name doesn't map to a
/// valid mouse_button.
boost::optional<mouse_button> mouse_button_from_string(const std::string& name);

} /* namespace glfw */
} /* namespace mce */

#endif /* MCE_GLFW_NAME_MAPPING_HPP_ */
