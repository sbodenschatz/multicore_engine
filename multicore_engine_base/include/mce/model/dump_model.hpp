/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/model/dump_model.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_MODEL_DUMP_MODEL_HPP_
#define MCE_MODEL_DUMP_MODEL_HPP_

#include <iostream>

namespace mce {
namespace model {

void dump_model_vertices(std::ostream& ostr, const void* vertex_data, size_t vertices);
void dump_model_vertices(std::ostream& ostr, const void* vertex_data, const void* index_data, size_t indices);

} /* namespace model */
} /* namespace mce */

#endif /* MCE_MODEL_DUMP_MODEL_HPP_ */
