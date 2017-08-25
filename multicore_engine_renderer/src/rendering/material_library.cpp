/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/material_library.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <mce/asset/asset.hpp>
#include <mce/rendering/material_library.hpp>
#include <sstream>

namespace mce {
namespace rendering {

material_library::~material_library() {}

void material_library::raise_error_flag(std::exception_ptr e) noexcept {
	current_state_ = state::error;
	std::unique_lock<std::mutex> lock(modification_mutex);
	for(auto& handler : error_handlers) {
		try {
			handler(e);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	error_handlers.clear();
	completion_handlers.clear();
	error_handlers.shrink_to_fit();
	completion_handlers.shrink_to_fit();
}

void material_library::complete_loading(const asset::asset_ptr& lib_asset) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	try {
		namespace pt = boost::property_tree;
		pt::ptree data_tree;
		std::stringstream str(std::string(lib_asset->data(), lib_asset->size()));
		pt::read_json(str, data_tree);
		for(auto& lib_entry : data_tree) {
			material_description entry;
			entry.name = lib_entry.first;
			entry.albedo_map_name = std::move(lib_entry.second.get_child("albedo").data());
			entry.normal_map_name = std::move(lib_entry.second.get_child("normal").data());
			entry.material_map_name = std::move(lib_entry.second.get_child("material").data());
			entry.emission_map_name = std::move(lib_entry.second.get_child("emission").data());
			if(entry.name.empty() || entry.albedo_map_name.empty() || entry.normal_map_name.empty() ||
			   entry.material_map_name.empty() || entry.emission_map_name.empty()) {
				throw mce::syntax_exception("Empty value not allowed in material definition.");
			}
			auto r = material_descriptions_.insert(std::make_pair(lib_entry.first, std::move(entry)));
			if(!r.second) {
				throw mce::syntax_exception("Duplicate name '" + lib_entry.first +
											"' for material definition.");
			}
		}
	} catch(...) {
		raise_error_flag(std::current_exception());
	}
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const material_library>(this->shared_from_this());
	lock.unlock();
	// From here on the polygon model object is immutable and can therefore be read without holding a lock
	for(auto& handler : completion_handlers) {
		try {
			handler(this_shared);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	completion_handlers.clear();
	error_handlers.clear();
	completion_handlers.shrink_to_fit();
	error_handlers.shrink_to_fit();
}

} /* namespace rendering */
} /* namespace mce */
