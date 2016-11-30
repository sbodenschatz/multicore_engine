/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/model/model_format.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_FORMAT_HPP_
#define MODEL_MODEL_FORMAT_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <string>
#include <util/composite_magic_number.hpp>
#include <util/offset_range.hpp>
#include <vector>

namespace mce {
namespace model {

struct model_vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

using model_index = uint32_t;

struct static_model_mesh_meta_data {
	std::string name;
	util::offset_range<uint64_t> index_data;
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_mesh_meta_data& value) {
		stream << value.name;
		stream << value.index_data;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_mesh_meta_data& value) {
		stream >> value.name;
		stream >> value.index_data;
		return stream;
	}
};

struct static_model_meta_data {
	constexpr static uint64_t magic_number_ =
			util::composite_magic_number<uint64_t>('m', 'c', 'e', 'm', 'o', 'd', 'e', 'l');
	constexpr static uint64_t version_ = util::composite_magic_number<uint64_t>(0u, 1u);

	uint64_t magic_number = magic_number_;
	uint64_t version = version_;
	std::vector<static_model_mesh_meta_data> meshes;
	util::offset_range<uint64_t> vertex_data;
	friend bstream::obstream& operator<<(bstream::obstream& stream, const static_model_meta_data& value) {
		stream << value.magic_number;
		stream << value.version;
		stream << value.meshes;
		stream << value.vertex_data;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_meta_data& value) {
		stream >> value.magic_number;
		stream >> value.version;
		stream >> value.meshes;
		stream >> value.vertex_data;
		if(value.magic_number != magic_number_) throw std::runtime_error("Invalid magic number.");
		if(value.version != version_) throw std::runtime_error("Can't load different model version.");
		return stream;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_FORMAT_HPP_ */
