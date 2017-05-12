/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/core/exceptions.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef CORE_EXCEPTIONS_HPP_
#define CORE_EXCEPTIONS_HPP_

/**
 * \file
 * Provides the definitions for exception classes used in the mce code base.
 */

#include <stdexcept>

namespace mce {

/// Exception used to signal an invalid magic number when loading binary data.
struct invalid_magic_number_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal an incompatible file format version when loading data.
struct invalid_version_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal errors from the (de-)compression library.
struct compression_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal problems with the size of supplied buffers.
struct buffer_size_exception : std::length_error {
	using std::length_error::length_error;
};
/// Exception used to signal that values exceed the allowed range.
struct out_of_range_exception : std::out_of_range {
	using std::out_of_range::out_of_range;
};
/// Exception used to signal that a path or file was not found.
struct path_not_found_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal general problems with I/O operations.
struct io_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal syntax errors in parsed files.
struct syntax_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that a requested functionality is not implemented.
struct unimplemented_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal errors in the program logic.
struct logic_exception : std::logic_error {
	using std::logic_error::logic_error;
};
/// Exception used to signal that an asynchronously used object was not in the required state for the request.
struct async_state_exception : std::logic_error {
	using std::logic_error::logic_error;
};
/// Exception used to signal that a value of an invalid type or an invalid literal was supplied.
struct value_type_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that an invalid property was referenced.
struct invalid_property_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that the referenced entity does not exist.
struct missing_entity_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that the referenced entity configuration does not exist.
struct missing_entity_config_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that an invalid component type was referenced.
struct invalid_component_type_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that duplicate component type was registered.
struct duplicate_component_type_exception : std::runtime_error {
	using std::runtime_error::runtime_error;
};
/// Exception used to signal that the desired access to a property is not supported.
struct invalid_property_access_exception : std::logic_error {
	using std::logic_error::logic_error;
};
/// Exception used to signal an operation that is not supported by an object in it's given configuration.
struct invalid_operation_exception : std::logic_error {
	using std::logic_error::logic_error;
};

/// Exception used to signal a conflict between different resolutions of the same config variable.
struct config_variable_conflict : std::logic_error {
	using std::logic_error::logic_error;
};

} // namespace mce

#endif /* CORE_EXCEPTIONS_HPP_ */
