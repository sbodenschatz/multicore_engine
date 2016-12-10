/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <array>
#include <asset_gen/obj_model_parser.hpp>
#include <boost/utility/string_view.hpp>
#include <fstream>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <util/string_tools.hpp>
#include <util/unused.hpp>

namespace mce {
namespace asset_gen {

bool obj_model_parser::check_prefix(boost::string_view str, boost::string_view prefix,
									boost::string_view& rest) const {
	if(util::starts_with(str, prefix)) {
		rest = str.substr(prefix.size());
		return true;
	} else {
		return false;
	}
}

// TODO: Try to implement these functions without copying to a temporary buffer
float obj_model_parser::stof(boost::string_view str, std::size_t* pos) {
	sto_buffer.clear();
	sto_buffer.append(str.data(), str.size());
	return std::stof(sto_buffer, pos);
}
long long obj_model_parser::stoll(boost::string_view str, std::size_t* pos) {
	sto_buffer.clear();
	sto_buffer.append(str.data(), str.size());
	return std::stoll(sto_buffer, pos);
}

void obj_model_parser::parse_file(const std::string& filename) {
	std::ifstream obj_file(filename);
	if(!obj_file) throw std::runtime_error("Couldn't open input file.");
	parse(obj_file);
}
void obj_model_parser::parse(std::istream& input) {
	for(std::string line_str; std::getline(input, line_str);) {
		boost::string_view line = line_str;

		auto trimmed_end = line.find_last_not_of(" \t");
		if(trimmed_end != line.npos) {
			++trimmed_end; // point to first whitespace char
			line.remove_suffix(line.size() - trimmed_end);
		}

		boost::string_view param;
		if(check_prefix(line, "v ", param)) {
			parse_vertex_position(param);
		} else if(check_prefix(line, "vt ", param)) {
			parse_vertex_texcoords(param);
		} else if(check_prefix(line, "vn ", param)) {
			parse_vertex_normal(param);
		} else if(check_prefix(line, "vp ", param)) {
			parse_vertex_parameter(param);
		} else if(check_prefix(line, "usemtl ", param)) {
			parse_usemtl(param);
		} else if(check_prefix(line, "mtllib ", param)) {
			parse_mtllib(param);
		} else if(check_prefix(line, "o ", param)) {
			parse_object(param);
		} else if(check_prefix(line, "g ", param)) {
			parse_group(param);
		} else if(check_prefix(line, "s ", param)) {
			parse_smoothing(param);
		} else if(check_prefix(line, "f ", param)) {
			parse_face(param);
		} else if(check_prefix(line, "#", param)) {
		} else {
			throw std::runtime_error("Unknown command: " + line_str);
		}
	}
}

void obj_model_parser::parse_vertex_position(boost::string_view line) {
	size_t i = 0;
	glm::vec3 pos;
	util::split_iterate(line, " ", [this, &pos, &i](boost::string_view e) {
		if(i >= 3) throw std::runtime_error("Too many components.");
		pos[i] = stof(e);
		++i;
	});
	positions.push_back(pos);
}
void obj_model_parser::parse_vertex_normal(boost::string_view line) {
	size_t i = 0;
	glm::vec3 normal;
	util::split_iterate(line, " ", [this, &normal, &i](boost::string_view e) {
		if(i >= 3) throw std::runtime_error("Too many components.");
		normal[i] = stof(e);
		++i;
	});
	normals.push_back(normal);
}
void obj_model_parser::parse_vertex_texcoords(boost::string_view line) {
	size_t i = 0;
	glm::vec2 uv;
	util::split_iterate(line, " ", [this, &uv, &i](boost::string_view e) {
		if(i >= 2) throw std::runtime_error("Too many components.");
		uv[i] = stof(e);
		++i;
	});
	tex_coords.push_back(uv);
}
void obj_model_parser::parse_vertex_parameter(boost::string_view) {
	throw std::runtime_error("Vertex Parameters not supported yet");
}
void obj_model_parser::parse_usemtl(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_object(boost::string_view line) {
	current_object_name.clear();
	current_object_name.append(line.data(), line.size());
}
void obj_model_parser::parse_mtllib(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_group(boost::string_view line) {
	current_group_name.clear();
	current_group_name.append(line.data(), line.size());
}
void obj_model_parser::parse_smoothing(boost::string_view line) {
	if(line != "off") {
		throw std::runtime_error("Smoothing groups are currently not supported.");
	}
}
void obj_model_parser::parse_face(boost::string_view line) {
	if(meshes.empty() || meshes.back().object_name != current_object_name ||
	   meshes.back().group_name != current_group_name) {
		meshes.emplace_back(current_object_name, current_group_name);
	}
	size_t face_vertex = 0;
	std::array<glm::ivec3, 3> vert_tripples;
	util::split_iterate(line, " ", [this, &face_vertex, &vert_tripples](boost::string_view vert_ref) {
		glm::ivec3 current_tripple;
		size_t elem_index = 0;
		util::split_iterate(
				vert_ref, "/", [this, &current_tripple, &elem_index](boost::string_view ref_elem) {
					if(elem_index > 2) {
						throw std::runtime_error("Too many elements in vertex reference.");
					}
					auto elem = stoll(ref_elem) - 1;
					if(elem > std::numeric_limits<glm::ivec3::value_type>::max()) {
						throw std::runtime_error("Numeric overflow in elements of vertex reference.");
					}
					current_tripple[elem_index] = glm::ivec3::value_type(elem);
					++elem_index;
				});
		meshes.back().collision_vertices.insert(current_tripple.x);
		if(face_vertex > 1) {
			vert_tripples[2] = current_tripple;
			create_face(vert_tripples);
			vert_tripples[1] = vert_tripples[2];
		} else {
			vert_tripples[face_vertex] = current_tripple;
		}
		++face_vertex;
	});
}

model::model_index obj_model_parser::get_or_create_vertex(const glm::ivec3& tripple) {
	auto it = vertex_indices.find(tripple);
	if(it != vertex_indices.end()) {
		return it->second;
	} else {
		model::model_vertex new_vert;
		new_vert.position = positions.at(tripple[0]);
		new_vert.tex_coords = tex_coords.at(tripple[1]);
		new_vert.normal = normals.at(tripple[2]);
		if(vertices.size() > std::numeric_limits<model::model_index>::max()) {
			throw std::runtime_error("Numeric overflow in index data.");
		}
		model::model_index index = model::model_index(vertices.size());
		vertices.push_back(new_vert);
		vertex_indices[tripple] = index;
		return index;
	}
}

void obj_model_parser::create_face(const std::array<glm::ivec3, 3>& vertex_tripples) {
	std::transform(vertex_tripples.begin(), vertex_tripples.end(), std::back_inserter(meshes.back().indices),
				   [this](const glm::ivec3& tripple) {
					   auto index = get_or_create_vertex(tripple);
					   return index;
				   });
}

std::tuple<static_model, model::static_model_collision_data> obj_model_parser::finalize_model() {
	for(auto& mesh : meshes) {
		mesh.collision_data.sphere.center =
				std::accumulate(mesh.collision_vertices.begin(), mesh.collision_vertices.end(),
								glm::vec3(.0f, .0f, .0f),
								[this](const auto& a, const auto& b) { return a + positions.at(b); }) *
				(1.0f / mesh.collision_vertices.size());
		mesh.collision_data.sphere.radius =
				sqrt(std::accumulate(mesh.collision_vertices.begin(), mesh.collision_vertices.end(), 0.0f,
									 [this, &mesh](const auto& a, const auto& b) {
										 auto r = vertices.at(b).position - mesh.collision_data.sphere.center;
										 return std::max(a, glm::dot(r, r));
									 }));
		auto fmax = std::numeric_limits<float>::max();
		auto vmax = glm::vec3(fmax, fmax, fmax);
		auto start = model::axis_aligned_collision_box(vmax, -vmax);
		mesh.collision_data.axis_aligned_box = std::accumulate(
				mesh.collision_vertices.begin(), mesh.collision_vertices.end(), start,
				[this](const auto& a, const auto& b) {
					const auto& p = vertices.at(b).position;
					return model::axis_aligned_collision_box(glm::min(a.min, p), glm::max(a.max, p));
				});
		mesh.collision_data.object_name = mesh.object_name;
		mesh.collision_data.group_name = mesh.group_name;
	}
	model::static_model_collision_data model_colision_data;
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model_colision_data.meshes),
				   [](const auto& m) { return m.collision_data; });

	static_model model;
	model.vertices = vertices;
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model.meshes), [](const auto& m) {
		static_model_mesh mesh;
		mesh.object_name = m.object_name;
		mesh.group_name = m.group_name;
		mesh.indices = m.indices;
		return mesh;
	});
	return std::make_tuple(std::move(model), std::move(model_colision_data));
}

std::vector<boost::filesystem::path> obj_model_parser::list_refs(const std::string& filename) const {
	std::vector<boost::filesystem::path> refs_list;
	std::ifstream obj_file(filename);
	if(!obj_file) throw std::runtime_error("Couldn't open input file.");

	for(std::string line_str; std::getline(obj_file, line_str);) {
		boost::string_view line = line_str;

		auto trimmed_end = line.find_last_not_of(" \t");
		if(trimmed_end != line.npos) {
			++trimmed_end; // point to first whitespace char
			line.remove_suffix(line.size() - trimmed_end);
		}

		boost::string_view param;
		if(check_prefix(line, "mtllib ", param)) {
			boost::filesystem::path mtllib_path(std::string(param.data(), param.size()));
			refs_list.push_back(boost::filesystem::absolute(mtllib_path, refs_dir));
		}
	}
	return refs_list;
}

} /* namespace asset_gen */
} /* namespace mce */
