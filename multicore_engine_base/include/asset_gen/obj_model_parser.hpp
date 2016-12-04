/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_OBJ_MODEL_PARSER_HPP_
#define ASSET_GEN_OBJ_MODEL_PARSER_HPP_

#include "static_model.hpp"
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/utility/string_view.hpp>
#include <glm/glm.hpp>
#include <model/model_format.hpp>
#include <tuple>
#include <utility>
#include <vector>

namespace mce {
namespace asset_gen {

class obj_model_parser {
private:
	class mesh_data {
	public:
		std::string object_name;
		std::string group_name;
		std::vector<model::model_index> indices;
		boost::container::flat_set<model::model_index> collision_vertices; // Contains each vertex used in the
																		   // mesh exactly once. Is used to
																		   // calculate collision_data.
		model::static_model_mesh_collision_data collision_data;

		mesh_data(const std::string& object_name, const std::string& group_name)
				: object_name{object_name}, group_name{group_name} {}
	};
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<model::model_vertex> vertices;
	std::vector<mesh_data> meshes;
	boost::container::flat_map<glm::ivec3, model::model_index> vertex_indices;
	bool indexed = true;
	std::string current_object_name;
	std::string current_group_name = "default";

	std::string sto_buffer;

	void parse_vertex_position(boost::string_view line);
	void parse_vertex_normal(boost::string_view line);
	void parse_vertex_texcoords(boost::string_view line);
	void parse_vertex_parameter(boost::string_view line);
	void parse_mtllib(boost::string_view line);
	void parse_usemtl(boost::string_view line);
	void parse_object(boost::string_view line);
	void parse_group(boost::string_view line);
	void parse_smoothing(boost::string_view line);
	void parse_face(boost::string_view line);

	bool check_prefix(boost::string_view str, boost::string_view prefix, boost::string_view& rest) const;

	// TODO: Try to implement these functions without copying to a temporary buffer
	float stof(boost::string_view str, std::size_t* pos = nullptr);
	long long stoll(boost::string_view str, std::size_t* pos = nullptr);

public:
	obj_model_parser(bool indexed = true) : indexed(indexed){};
	void parse_file(const std::string& filename);
	std::tuple<static_model, model::static_model_collision_data> finalize_model();
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_OBJ_MODEL_PARSER_HPP_ */
