/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/model_exporter.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <asset_gen/model_exporter.hpp>
#include <util/unused.hpp>

namespace mce {
namespace asset_gen {

void model_exporter::export_model(const model& model_data, const std::string& output_file) const {
	// TODO: Implement
	UNUSED(model_data);
	UNUSED(output_file);
}
void model_exporter::export_model(const model_collision_data& collision_data,
								  const std::string& output_file) const {
	// TODO: Implement
	UNUSED(collision_data);
	UNUSED(output_file);
}

} /* namespace asset_gen */
} /* namespace mce */
