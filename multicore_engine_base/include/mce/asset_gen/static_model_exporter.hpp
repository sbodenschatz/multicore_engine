/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/asset_gen/model_exporter.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_
#define ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_

/**
 * \file
 * Defines a class implementing an exporter for an engine specific static model format.
 */

#include <string>

namespace mce {
namespace model {
struct static_model_collision_data;
} // namespace model

namespace asset_gen {
struct static_model;

/// Implements the functionality to export models into the engine format for polygon and collision models.
class static_model_exporter {
public:
	/// Export the given static model into the given model file to be later used for rendering in the engine.
	void export_model(const static_model& model_data, const std::string& output_file) const;
	/// Export the given static model collision data into the given file for usage as collision geometry.
	void export_model(const model::static_model_collision_data& collision_data,
					  const std::string& output_file) const;
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_STATIC_MODEL_EXPORTER_HPP_ */
