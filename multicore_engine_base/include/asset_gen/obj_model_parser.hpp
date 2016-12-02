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
		std::string name;
		std::vector<model::model_index> indices;
		boost::container::flat_set<model::model_index> collision_vertices; // Contains each vertex used in the
																		   // mesh exactly once. Is used to
																		   // calculate collision_data.
		model::static_model_mesh_collision_data collision_data;
	};
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<model::model_vertex> vertices;
	std::vector<mesh_data> meshes;
	boost::container::flat_map<glm::ivec3, model::model_index> vertex_indices;
	bool indexed = true;

	void parse_vertex_position(const std::string& line);
	void parse_vertex_normal(const std::string& line);
	void parse_vertex_texcoords(const std::string& line);
	void parse_vertex_parameter(const std::string& line);
	void parse_mtllib(const std::string& line);
	void parse_usemtl(const std::string& line);
	void parse_object(const std::string& line);
	void parse_group(const std::string& line);
	void parse_smoothing(const std::string& line);
	void parse_face(const std::string& line);

	bool check_prefix(const std::string& str, const std::string& prefix, std::string& rest) const;

public:
	obj_model_parser(bool indexed = true) : indexed(indexed){};
	void parse_file(const std::string& filename);
	std::tuple<static_model, model::static_model_collision_data> finalize_model();
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_OBJ_MODEL_PARSER_HPP_ */
