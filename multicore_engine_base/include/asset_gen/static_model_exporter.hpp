/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/model_exporter.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_
#define ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_

#include "static_model.hpp"
#include <string>

namespace mce {
namespace asset_gen {

class static_model_exporter {
public:
	void export_model(const static_model& model_data, const std::string& output_file) const;
	void export_model(const model::static_model_collision_data& collision_data,
					  const std::string& output_file) const;
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_ */
