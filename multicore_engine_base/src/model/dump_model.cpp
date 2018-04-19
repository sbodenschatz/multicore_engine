/*
 * Multi-Core Engine project
 * File /multicore_engine_base/src/model/dump_model.cpp
 * Copyright 2017-2018 by Stefan Bodenschatz
 */

#include <cstring>
#include <mce/model/dump_model.hpp>
#include <mce/model/model_format.hpp>

// Workaround for compile error in glm/gtx/dual_quaternion.hpp:
// TODO Real fix if upstream doesn't fix it
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_precision.hpp>
#ifdef GLM_FORCE_CTOR_INIT
#define GLM_FORCE_CTOR_INIT_ORIG
#endif
#undef GLM_FORCE_CTOR_INIT
#include <glm/gtx/string_cast.hpp>
#ifdef GLM_FORCE_CTOR_INIT_ORIG
#define GLM_FORCE_CTOR_INIT
#endif

namespace mce {
namespace model {

void dump_model_vertices(std::ostream& ostr, const void* vertex_data, const void* index_data,
						 size_t indices) {
	const char* vert = static_cast<const char*>(vertex_data);
	const char* ind = static_cast<const char*>(index_data);
	for(size_t i = 0; i < indices; ++i) {
		uint32_t cur_ind = 0;
		std::memcpy(&cur_ind, ind + (i * sizeof(uint32_t)), sizeof(uint32_t));
		model_vertex mv;
		std::memcpy(&mv, vert + (cur_ind * sizeof(model_vertex)), sizeof(model_vertex));
		ostr << cur_ind << "\t| " << glm::to_string(mv.position) << "\t| " << glm::to_string(mv.normal)
			 << "\t| " << glm::to_string(mv.tex_coords) << "\n";
		if(i % 3 == 2) std::cout << "\n";
	}
}

void dump_model_vertices(std::ostream& ostr, const void* vertex_data, size_t vertices) {
	const char* vert = static_cast<const char*>(vertex_data);
	for(size_t i = 0; i < vertices; ++i) {
		model_vertex mv;
		std::memcpy(&mv, vert + (i * sizeof(model_vertex)), sizeof(model_vertex));
		ostr << glm::to_string(mv.position) << "\t| " << glm::to_string(mv.normal) << "\t| "
			 << glm::to_string(mv.tex_coords) << "\n";
		if(i % 3 == 2) std::cout << "\n";
	}
}

} /* namespace model */
} /* namespace mce */
