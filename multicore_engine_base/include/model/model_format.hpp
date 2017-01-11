/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/model/model_format.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
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

/// Defines the structure of a vertex in a model.
struct model_vertex {
	glm::vec3 position;   ///< The position in 3D model space.
	glm::vec3 normal;	 ///< The normal vector of the vertex.
	glm::vec2 tex_coords; ///< The mapped texture coordinates of the vertex.
};

/// Defines the type used for model index data.
using model_index = uint32_t;

/// Represents the meta data for a single static mesh of a static model.
struct static_model_mesh_meta_data {
	std::string object_name; ///< The object name of the static mesh.
	std::string group_name;  ///< The group name of the static mesh.

	/// The offset range of the index data of the static mesh within the model file.
	util::offset_range<uint64_t> index_data;

	/// Serializes the mesh meta data.
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_mesh_meta_data& value) {
		stream << value.object_name;
		stream << value.group_name;
		stream << value.index_data;
		return stream;
	}

	/// Deserializes the mesh meta data.
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_mesh_meta_data& value) {
		stream >> value.object_name;
		stream >> value.group_name;
		stream >> value.index_data;
		return stream;
	}
};

/// Represents the meta data for a static model consisting of arbitrarily many static meshes.
struct static_model_meta_data {
	/// The magic number to identify model files.
	constexpr static uint64_t magic_number_ = util::composite_magic_number<uint64_t>(
			'm', 'c', 'e', 'm', 'd', 'l', 's' /*static*/, 'g' /*geometry*/);
	/// The supported(current) version of the model file format.
	constexpr static uint64_t version_ = util::composite_magic_number<uint64_t>(0u, 1u);

	uint64_t magic_number = magic_number_;			 ///< The deserialized magic number.
	uint64_t version = version_;					 ///< The deserialized version tag.
	std::vector<static_model_mesh_meta_data> meshes; ///< The meta data for the meshes in the model.

	/// The offset range of the vertex data of the static model within the model file.
	util::offset_range<uint64_t> vertex_data;

	/// Serializes the model meta data.
	friend bstream::obstream& operator<<(bstream::obstream& stream, const static_model_meta_data& value) {
		stream << value.magic_number;
		stream << value.version;
		stream << value.meshes;
		stream << value.vertex_data;
		return stream;
	}

	/// Deserializes the model meta data.
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

/// Defines the structure of sphere-based collision data.
struct collision_sphere {
	glm::vec3 center; ///< The center point of the sphere.
	float radius;	 ///< The radius of the sphere.

	/// Serializes the sphere collision data.
	friend bstream::obstream& operator<<(bstream::obstream& stream, const collision_sphere& value) {
		stream << value.center;
		stream << value.radius;
		return stream;
	}
	/// Deserializes the sphere collision data.
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, collision_sphere& value) {
		stream >> value.center;
		stream >> value.radius;
		return stream;
	}
};

/// Defines the structure of collision data based on axis aligned bounding boxes.
struct axis_aligned_collision_box {
	glm::vec3 min; ///< The vertex of the box that has the minimum value of all axis.
	glm::vec3 max; ///< The vertex of the box that has the maximum value of all axis.

	/// Constructs an axis aligned collision box.
	axis_aligned_collision_box(glm::vec3 min = glm::vec3(.0f, .0f, .0f),
							   glm::vec3 max = glm::vec3(.0f, .0f, .0f))
			: min{min}, max{max} {}

	/// Calculates the center point of the box.
	glm::vec3 center() const {
		return (max + min) * 0.5f;
	}
	/// Serializes the axis aligned collision box data.
	friend bstream::obstream& operator<<(bstream::obstream& stream, const axis_aligned_collision_box& value) {
		stream << value.min;
		stream << value.max;
		return stream;
	}
	/// Deserializes the axis aligned collision box data.
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, axis_aligned_collision_box& value) {
		stream >> value.min;
		stream >> value.max;
		return stream;
	}
};

/// Defines the structure of the collision data for a mesh of a static model.
struct static_model_mesh_collision_data {
	std::string object_name;					 ///< The object name of the mesh.
	std::string group_name;						 ///< The group name of the mesh.
	collision_sphere sphere;					 ///< The sphere based collision data for the mesh.
	axis_aligned_collision_box axis_aligned_box; ///< The axis-aligned box based collision data for the mesh.

	/// Serializes the static mesh collision data.
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_mesh_collision_data& value) {
		stream << value.object_name;
		stream << value.group_name;
		stream << value.sphere;
		stream << value.axis_aligned_box;
		return stream;
	}
	/// Deserializes the static mesh collision data.
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, static_model_mesh_collision_data& value) {
		stream >> value.object_name;
		stream >> value.group_name;
		stream >> value.sphere;
		stream >> value.axis_aligned_box;
		return stream;
	}
};

/// Defines the structure of the collision data for a static model.
struct static_model_collision_data {
	/// The magic number to identify collision data files.
	constexpr static uint64_t magic_number_ = util::composite_magic_number<uint64_t>(
			'm', 'c', 'e', 'm', 'd', 'l', 's' /*static*/, 'c' /*collision*/);
	/// The supported(current) version of the collision data files.
	constexpr static uint64_t version_ = util::composite_magic_number<uint64_t>(0u, 1u);

	uint64_t magic_number = magic_number_;				  ///< The deserialized magic number.
	uint64_t version = version_;						  ///< The deserialized version flag.
	std::vector<static_model_mesh_collision_data> meshes; ///<The collision data of the meshes of the model.

	/// Serializes the static model collision data.
	friend bstream::obstream& operator<<(bstream::obstream& stream,
										 const static_model_collision_data& value) {
		stream << value.magic_number;
		stream << value.version;
		stream << value.meshes;
		return stream;
	}
	/// Deserializes the static model collision data.
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
