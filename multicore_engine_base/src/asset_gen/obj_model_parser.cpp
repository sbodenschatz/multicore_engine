/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset_gen/obj_model_parser.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <asset_gen/obj_model_parser.hpp>
#include <iterator>
#include <limits>
#include <numeric>
#include <util/unused.hpp>

namespace mce {
namespace asset_gen {

void obj_model_parser::parse_file(const std::string& filename) {
	// TODO: Implement
	UNUSED(filename);
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
