/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
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

void obj_model_parser::parse_file(const std::string& filename) {
	std::ifstream obj_file(filename);
	if(!obj_file) throw std::runtime_error("Couldn't open input file.");

	for(std::string line_str; std::getline(obj_file, line_str);) {
		boost::string_view line = line_str;

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
		} else if(!std::all_of(line.begin(), line.end(), [](char c) { return c == ' ' || c == '\t'; })) {
			throw std::runtime_error("Unknown command: " + line_str);
		}
	}
}

void obj_model_parser::parse_vertex_position(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_vertex_normal(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_vertex_texcoords(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_vertex_parameter(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_usemtl(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_object(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_mtllib(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_group(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_smoothing(boost::string_view line) {
	UNUSED(line);
}
void obj_model_parser::parse_face(boost::string_view line) {
	UNUSED(line);
}

std::tuple<static_model, model::static_model_collision_data> obj_model_parser::finalize_model() {
	for(auto& mesh : meshes) {
		mesh.collision_data.sphere.center =
				std::accumulate(
						mesh.collision_vertices.begin(), mesh.collision_vertices.end(),
						glm::vec3(.0f, .0f, .0f),
						[this](const auto& a, const auto& b) { return a + vertices.at(b).position; }) *
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
		mesh.collision_data.name = mesh.name;
	}
	model::static_model_collision_data model_colision_data;
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model_colision_data.meshes),
				   [](const auto& m) { return m.collision_data; });

	static_model model;
	model.vertices = vertices;
	std::transform(meshes.begin(), meshes.end(), std::back_inserter(model.meshes), [](const auto& m) {
		static_model_mesh mesh;
		mesh.name = m.name;
		mesh.indices = m.indices;
		return mesh;
	});
	return std::make_tuple(std::move(model), std::move(model_colision_data));
}

} /* namespace asset_gen */
} /* namespace mce */
