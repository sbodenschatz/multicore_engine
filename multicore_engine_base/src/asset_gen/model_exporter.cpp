/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/asset_gen/model_exporter.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mce/asset_gen/static_model.hpp>
#include <mce/asset_gen/static_model_exporter.hpp>
#include <mce/bstream/iostream_bstream.hpp>
#include <mce/bstream/vector_iobstream.hpp>
#include <mce/exceptions.hpp>
#include <mce/model/model_format.hpp>
#include <mce/util/offset_range.hpp>
#include <mce/util/unused.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {

void static_model_exporter::export_model(const static_model& model_data,
										 const std::string& output_file) const {
	model::static_model_meta_data model_meta_data;
	model_meta_data.vertex_data_in_content.length(model_data.vertices.size() *
												  sizeof(decltype(model_data.vertices)::value_type));
	auto next_offset = model_meta_data.vertex_data_in_content.length();
	model_meta_data.meshes.reserve(model_data.meshes.size());
	std::transform(model_data.meshes.begin(), model_data.meshes.end(),
				   std::back_inserter(model_meta_data.meshes), [&next_offset](const static_model_mesh& mesh) {
					   model::static_model_mesh_meta_data meta_data;
					   meta_data.object_name = mesh.object_name;
					   meta_data.group_name = mesh.group_name;
					   meta_data.material_name = mesh.material_name;
					   meta_data.index_data_in_content.begin(next_offset);
					   meta_data.index_data_in_content.length(mesh.indices.size() *
															  sizeof(decltype(mesh.indices)::value_type));
					   next_offset += meta_data.index_data_in_content.length();
					   return meta_data;
				   });

	bstream::vector_iobstream tmp_stream;
	tmp_stream << model_meta_data;
	uint64_t header_size = tmp_stream.tell_write();
	model_meta_data.content_range.begin(header_size);
	model_meta_data.content_range.length(next_offset);

	std::fstream file_stream(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!file_stream) throw path_not_found_exception("Can't open '" + output_file + "' for writing.");
	bstream::iostream_bstream stream(file_stream);
	stream << model_meta_data;
	assert(stream.tell_write() ==
		   model_meta_data.vertex_data_in_content.begin() + model_meta_data.content_range.begin());
	stream.write_bytes(reinterpret_cast<const char*>(model_data.vertices.data()),
					   sizeof(decltype(model_data.vertices)::value_type) * model_data.vertices.size());
	assert(stream.tell_write() ==
		   model_meta_data.vertex_data_in_content.end() + model_meta_data.content_range.begin());
	for(size_t i = 0; i < model_data.meshes.size(); ++i) {
		auto& mesh = model_data.meshes[i];
		auto& mesh_meta = model_meta_data.meshes[i];
		UNUSED(mesh_meta);
		assert(stream.tell_write() ==
			   mesh_meta.index_data_in_content.begin() + model_meta_data.content_range.begin());
		stream.write_bytes(reinterpret_cast<const char*>(mesh.indices.data()),
						   sizeof(decltype(mesh.indices)::value_type) * mesh.indices.size());
		assert(stream.tell_write() ==
			   mesh_meta.index_data_in_content.end() + model_meta_data.content_range.begin());
	}
}

void static_model_exporter::export_model(const model::static_model_collision_data& collision_data,
										 const std::string& output_file) const {
	std::fstream file_stream(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!file_stream) throw path_not_found_exception("Can't open '" + output_file + "' for writing.");
	bstream::iostream_bstream stream(file_stream);
	stream << collision_data;
}

} /* namespace asset_gen */
} /* namespace mce */
