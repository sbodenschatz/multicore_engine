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
#include <boost/filesystem.hpp>
#include <boost/utility/string_view.hpp>
#include <glm/glm.hpp>
#include <istream>
#include <model/model_format.hpp>
#include <tuple>
#include <utility>
#include <vector>

namespace mce {
namespace asset_gen {

class obj_model_parser {
private:
	struct tripple_comparator {
		bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
			return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
		}
	};
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
				: object_name{object_name}, group_name{group_name}, collision_data{} {}
	};
	boost::filesystem::path refs_dir;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<model::model_vertex> vertices;
	std::vector<mesh_data> meshes;
	boost::container::flat_map<glm::ivec3, model::model_index, tripple_comparator> vertex_indices;
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

	model::model_index get_or_create_vertex(const glm::ivec3& tripple);
	void create_face(const std::array<glm::ivec3, 3>& vertex_tripples);

	bool check_prefix(boost::string_view str, boost::string_view prefix, boost::string_view& rest) const;

	// TODO: Try to implement these functions without copying to a temporary buffer
	float stof(boost::string_view str, std::size_t* pos = nullptr);
	long long stoll(boost::string_view str, std::size_t* pos = nullptr);

public:
	explicit obj_model_parser(boost::filesystem::path refs_dir) : refs_dir(refs_dir){};
	void parse_file(const std::string& filename);
	void parse(std::istream& input);
	std::tuple<static_model, model::static_model_collision_data> finalize_model();
	std::vector<boost::filesystem::path> list_refs(const std::string& filename) const;
};

} /* namespace asset_gen */
} /* namespace mce */

#endif /* ASSET_GEN_OBJ_MODEL_PARSER_HPP_ */