/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/asset_gen/obj_model_parser.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <array>
#include <boost/container/vector.hpp>
#include <boost/filesystem.hpp>
#include <boost/utility/string_view.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <mce/asset_gen/obj_model_parser.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/string_tools.hpp>
#include <mce/util/unused.hpp>
#include <numeric>
#include <string>

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
	if(!obj_file) throw path_not_found_exception("Couldn't open input file.");
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
			throw syntax_exception("Unknown command: " + line_str);
		}
	}
}

void obj_model_parser::parse_vertex_position(boost::string_view line) {
	size_t i = 0;
	glm::vec3 pos;
	util::split_iterate(line, " ", [this, &pos, &i](boost::string_view e) {
		// cppcheck-suppress knownConditionTrueFalse
		if(i >= 3) throw syntax_exception("Too many components.");
		pos[i] = stof(e);
		++i;
	});
	positions.push_back(pos);
}
void obj_model_parser::parse_vertex_normal(boost::string_view line) {
	size_t i = 0;
	glm::vec3 normal;
	util::split_iterate(line, " ", [this, &normal, &i](boost::string_view e) {
		// cppcheck-suppress knownConditionTrueFalse
		if(i >= 3) throw syntax_exception("Too many components.");
		normal[i] = stof(e);
		++i;
	});
	normals.push_back(normal);
}
void obj_model_parser::parse_vertex_texcoords(boost::string_view line) {
	size_t i = 0;
	glm::vec2 uv;
	util::split_iterate(line, " ", [this, &uv, &i](boost::string_view e) {
		// cppcheck-suppress knownConditionTrueFalse
		if(i >= 2) throw syntax_exception("Too many components.");
		uv[i] = stof(e);
		++i;
	});
	tex_coords.push_back(uv);
}
void obj_model_parser::parse_vertex_parameter(boost::string_view) {
	throw unimplemented_exception("Vertex parameters not supported yet");
}
void obj_model_parser::parse_usemtl(boost::string_view line) {
	current_material_name.clear();
	current_material_name.append(line.data(), line.size());
}
void obj_model_parser::parse_object(boost::string_view line) {
	current_object_name.clear();
	current_object_name.append(line.data(), line.size());
}
void obj_model_parser::parse_mtllib(boost::string_view line) {
	// Material libraries are ignored because materials are defined in an engine specific json-based material
	// definition library format.
	UNUSED(line);
}
void obj_model_parser::parse_group(boost::string_view line) {
	current_group_name.clear();
	current_group_name.append(line.data(), line.size());
}
void obj_model_parser::parse_smoothing(boost::string_view line) {
	// Smoothing groups are ignored because the model format requires vertex normals and vertex normals
	// override smoothing groups.
	UNUSED(line);
}
void obj_model_parser::parse_face(boost::string_view line) {
	if(current_material_name.empty()) {
		throw syntax_exception("Attempt to create face without a defined material.");
	}
	if(meshes.empty() || meshes.back().object_name != current_object_name ||
	   meshes.back().group_name != current_group_name ||
	   meshes.back().material_name != current_material_name) {
		meshes.emplace_back(current_object_name, current_group_name, current_material_name);
	}
	size_t face_vertex = 0;
	std::array<glm::ivec3, 3> vert_tripples;
	util::split_iterate(line, " ", [this, &face_vertex, &vert_tripples](boost::string_view vert_ref) {
		glm::ivec3 current_tripple;
		size_t elem_index = 0;
		util::split_iterate(
				vert_ref, "/", [this, &current_tripple, &elem_index](boost::string_view ref_elem) {
					// cppcheck-suppress knownConditionTrueFalse
					if(elem_index > 2) {
						throw syntax_exception("Too many elements in vertex reference.");
					}
					auto elem = stoll(ref_elem) - 1;
					if(elem > std::numeric_limits<glm::ivec3::value_type>::max()) {
						throw out_of_range_exception("Numeric overflow in elements of vertex reference.");
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
			throw out_of_range_exception("Numeric overflow in index data.");
		}
		model::model_index index = model::model_index(vertices.size());
		vertices.push_back(new_vert);
		vertex_indices[tripple] = index;
		return index;
	}
}

void obj_model_parser::create_face(const std::array<glm::ivec3, 3>& vertex_tripples) {
	auto tans = calculate_tangents(vertex_tripples);
	std::transform(vertex_tripples.begin(), vertex_tripples.end(), std::back_inserter(meshes.back().indices),
				   [this, &tans](const glm::ivec3& tripple) {
					   auto index = get_or_create_vertex(tripple);
					   tangents[glm::ivec2(tripple.x, tripple.z)] += tans.first;
					   bitangents[glm::ivec2(tripple.x, tripple.z)] += tans.second;
					   return index;
				   });
}

std::pair<glm::vec3, glm::vec3>
obj_model_parser::calculate_tangents(const std::array<glm::ivec3, 3>& vertex_tripples) {
	std::array<glm::vec3, 3> p = {{positions.at(vertex_tripples[0].x), positions.at(vertex_tripples[1].x),
								   positions.at(vertex_tripples[2].x)}};
	std::array<glm::vec2, 3> uv = {{tex_coords.at(vertex_tripples[0].y), tex_coords.at(vertex_tripples[1].y),
									tex_coords.at(vertex_tripples[2].y)}};
	std::array<glm::vec3, 2> q = {{p[1] - p[0], p[2] - p[0]}};
	std::array<glm::vec2, 2> duv = {{uv[1] - uv[0], uv[2] - uv[0]}};
	glm::mat2 m_duv_inv = (1.0f / (duv[0].s * duv[1].t - duv[1].s * duv[0].t)) *
						  glm::mat2(duv[1].t, -duv[1].s, -duv[0].t, duv[0].s);
	glm::mat3x2 m_q = glm::mat3x2(q[0].x, q[1].x, q[0].y, q[1].y, q[0].z, q[1].z);
	glm::mat2x3 m_t = transpose(m_duv_inv * m_q);
	return {m_t[0], m_t[1]};
}

std::tuple<static_model, model::static_model_collision_data> obj_model_parser::finalize_model() {
	for(const auto& vert_idx : vertex_indices) {
		auto& vert = vertices[vert_idx.second];
		auto it_tan = tangents.find(glm::ivec2(vert_idx.first.x, vert_idx.first.z));
		assert(it_tan != tangents.end());
		auto it_bitan = bitangents.find(glm::ivec2(vert_idx.first.x, vert_idx.first.z));
		assert(it_bitan != bitangents.end());
		vert.tangent = glm::normalize(it_tan->second);
		vert.bitangent = glm::normalize(it_bitan->second);
	}
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
		mesh.collision_data.material_name = mesh.material_name;
	}
	model::static_model_collision_data model_colision_data;
	model_colision_data.meshes.reserve(meshes.size());
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model_colision_data.meshes),
				   [](const auto& m) { return m.collision_data; });

	static_model model;
	model.vertices = vertices;
	model.meshes.reserve(meshes.size());
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model.meshes), [](const auto& m) {
		static_model_mesh mesh;
		mesh.object_name = m.object_name;
		mesh.group_name = m.group_name;
		mesh.material_name = m.material_name;
		mesh.indices = m.indices;
		return mesh;
	});
	return std::make_tuple(std::move(model), std::move(model_colision_data));
}

std::vector<boost::filesystem::path> obj_model_parser::list_refs(const std::string& filename) const {
	std::vector<boost::filesystem::path> refs_list;
	std::ifstream obj_file(filename);
	if(!obj_file) throw path_not_found_exception("Couldn't open input file.");

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
