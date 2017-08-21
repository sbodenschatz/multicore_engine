/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/model/dump_model.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_MODEL_DUMP_MODEL_HPP_
#define MCE_MODEL_DUMP_MODEL_HPP_

/**
 * \file
 * Defines debug helper functions that allow dumping model geometry data.
 */

#include <iostream>

namespace mce {
namespace model {

/// \brief Dumps the model geometry information in the given number of vertices starting at the given address
/// to the given stream.
/**
 * Accesses memory through memcpy to avoid strict aliasing violations and problems caused by unaligned access.
 */
void dump_model_vertices(std::ostream& ostr, const void* vertex_data, size_t vertices);
/// \brief Dumps the model geometry information represented in the vertex and index data memory location for
/// the given number of indices to the given stream.
/**
 * Accesses memory through memcpy to avoid strict aliasing violations and problems caused by unaligned access.
 */
void dump_model_vertices(std::ostream& ostr, const void* vertex_data, const void* index_data, size_t indices);

} /* namespace model */
} /* namespace mce */

#endif /* MCE_MODEL_DUMP_MODEL_HPP_ */
