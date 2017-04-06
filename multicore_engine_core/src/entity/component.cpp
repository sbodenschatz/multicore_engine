/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component.cpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#include <entity/component.hpp>
#include <util/unused.hpp>
#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>

namespace mce {
namespace entity {

void component::fill_property_list(property_list&) {}

void component::store_to_bstream(bstream::obstream& ostr, bool compatible) const {
	for(const auto& prop : configuration_.type().properties()) {
		if(compatible) {
			ostr << prop->name();
			ostr << prop->type();
		}
		prop->to_bstream(*this, ostr);
	}
}
void component::load_from_bstream(bstream::ibstream& istr, bool compatible) {
	UNUSED(istr);
	UNUSED(compatible);
}

} // namespace entity
} // namespace mce
