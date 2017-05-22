/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component.cpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#include <mce/entity/component.hpp>
#include <mce/entity/component_configuration.hpp>
#include <mce/entity/component_type.hpp>

namespace mce {
namespace entity {

void component::fill_property_list(property_list&) {}

void component::store_to_bstream(bstream::obstream& ostr) const {
	for(const auto& prop : configuration_.type().properties()) {
		prop->to_bstream(*this, ostr);
	}
}
void component::load_from_bstream(bstream::ibstream& istr) {
	for(const auto& prop : configuration_.type().properties()) {
		prop->from_bstream(*this, istr);
	}
}

} // namespace entity
} // namespace mce
