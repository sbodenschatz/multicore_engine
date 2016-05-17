/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <asset_gen/obj_model_parser.hpp>
#include <util/unused.hpp>

namespace mce {
namespace asset_gen {

void obj_model_parser::parse_file(const std::string& filename) {
	// TODO: Implement
	UNUSED(filename);
}
std::tuple<model, model_collision_data> obj_model_parser::finalize_model() {
	// TODO: Implement
	return std::make_tuple(model(), model_collision_data());
}

} /* namespace asset_gen */
} /* namespace mce */
