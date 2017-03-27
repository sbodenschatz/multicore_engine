/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/core/exceptions.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef CORE_EXCEPTIONS_HPP_
#define CORE_EXCEPTIONS_HPP_

#include <stdexcept>

namespace mce {

struct invalid_magic_number_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct invalid_version_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct compression_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct buffer_size_exception : std::length_error {
	using std::length_error::length_error;
};

struct out_of_range_exception : std::out_of_range {
	using std::out_of_range::out_of_range;
};

struct path_not_found_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct io_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct syntax_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct unimplemented_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct logic_exception : std::logic_error {
	using std::logic_error::logic_error;
};

struct async_state_exception : std::logic_error {
	using std::logic_error::logic_error;
};

struct value_type_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct invalid_property_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct missing_entity_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct missing_entity_config_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};

struct invalid_component_type_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that the desired access to a property is not supported.
struct invalid_property_access_exception : std::logic_error {
	using std::logic_error::logic_error;
};

} // namespace mce

#endif /* CORE_EXCEPTIONS_HPP_ */
