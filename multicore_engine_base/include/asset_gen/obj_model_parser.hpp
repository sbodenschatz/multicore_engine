/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_OBJ_MODEL_PARSER_HPP_
#define ASSET_GEN_OBJ_MODEL_PARSER_HPP_

#include "model.hpp"
#include <boost/container/flat_map.hpp>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

namespace mce {
namespace asset_gen {

class obj_model_parser {
private:
	class mesh_data {
	public:
		std::string name;
		std::vector<model_index> indices;
	};
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<model_vertex> vertices;
	std::vector<mesh_data> meshes;
	boost::container::flat_map<glm::ivec3, model_index> vertex_indices;
	bool indexed = true;

public:
	obj_model_parser(bool indexed = true) : indexed(indexed){};
	void parse_file(const std::string& filename);
	std::tuple<model, model_collision_data> finalize_model();
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_OBJ_MODEL_PARSER_HPP_ */
