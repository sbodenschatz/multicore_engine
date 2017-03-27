/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/model/model_format.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_FORMAT_HPP_
#define MODEL_MODEL_FORMAT_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <exceptions.hpp>
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
	std::string object_name;
	std::string group_name;
	util::offset_range<uint64_t> index_data;
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_mesh_meta_data& value) {
		stream << value.object_name;
		stream << value.group_name;
		stream << value.index_data;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_mesh_meta_data& value) {
		stream >> value.object_name;
		stream >> value.group_name;
		stream >> value.index_data;
		return stream;
	}
};

struct static_model_meta_data {
	constexpr static uint64_t magic_number_ = util::composite_magic_number<uint64_t>(
			'm', 'c', 'e', 'm', 'd', 'l', 's' /*static*/, 'g' /*geometry*/);
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
		if(value.magic_number != magic_number_) throw invalid_magic_number_exception("Invalid magic number.");
		if(value.version != version_) throw invalid_version_exception("Can't load different model version.");
		return stream;
	}
};

struct collision_sphere {
	glm::vec3 center;
	float radius;
	friend bstream::obstream& operator<<(bstream::obstream& stream, const collision_sphere& value) {
		stream << value.center;
		stream << value.radius;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, collision_sphere& value) {
		stream >> value.center;
		stream >> value.radius;
		return stream;
	}
};

struct axis_aligned_collision_box {
	glm::vec3 min;
	glm::vec3 max;
	axis_aligned_collision_box(glm::vec3 min = glm::vec3(.0f, .0f, .0f),
							   glm::vec3 max = glm::vec3(.0f, .0f, .0f))
			: min{min}, max{max} {}
	glm::vec3 center() const {
		return (max + min) * 0.5f;
	}
	friend bstream::obstream& operator<<(bstream::obstream& stream, const axis_aligned_collision_box& value) {
		stream << value.min;
		stream << value.max;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, axis_aligned_collision_box& value) {
		stream >> value.min;
		stream >> value.max;
		return stream;
	}
};

struct static_model_mesh_collision_data {
	std::string object_name;
	std::string group_name;
	collision_sphere sphere;
	axis_aligned_collision_box axis_aligned_box;
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_mesh_collision_data& value) {
		stream << value.object_name;
		stream << value.group_name;
		stream << value.sphere;
		stream << value.axis_aligned_box;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_mesh_collision_data& value) {
		stream >> value.object_name;
		stream >> value.group_name;
		stream >> value.sphere;
		stream >> value.axis_aligned_box;
		return stream;
	}
};

struct static_model_collision_data {
	constexpr static uint64_t magic_number_ = util::composite_magic_number<uint64_t>(
			'm', 'c', 'e', 'm', 'd', 'l', 's' /*static*/, 'c' /*collision*/);
	constexpr static uint64_t version_ = util::composite_magic_number<uint64_t>(0u, 1u);

	uint64_t magic_number = magic_number_;
	uint64_t version = version_;
	std::vector<static_model_mesh_collision_data> meshes;

	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_collision_data& value) {
		stream << value.magic_number;
		stream << value.version;
		stream << value.meshes;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_collision_data& value) {
		stream >> value.magic_number;
		stream >> value.version;
		stream >> value.meshes;
		if(value.magic_number != magic_number_) throw invalid_magic_number_exception("Invalid magic number.");
		if(value.version != version_) throw invalid_version_exception("Can't load different model version.");
		return stream;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_FORMAT_HPP_ */
