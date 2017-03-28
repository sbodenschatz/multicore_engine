/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/reflection/type.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <reflection/type.hpp>

namespace mce {
namespace reflection {

std::string type_parser<std::vector<std::string>>::delimiter = ";";

} // namespace reflection
} // namespace mce
