/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/glfw/name_mapping.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GLFW_NAME_MAPPING_HPP_
#define MCE_GLFW_NAME_MAPPING_HPP_

#include <boost/optional.hpp>
#include <mce/glfw/glfw_defs.hpp>
#include <string>

namespace mce {
namespace glfw {

const std::string& to_string(key k);
key key_from_string(const std::string& name);

const std::string& to_string(mouse_button b);
boost::optional<mouse_button> mouse_button_from_string(const std::string& name);

} /* namespace glfw */
} /* namespace mce */

#endif /* MCE_GLFW_NAME_MAPPING_HPP_ */
