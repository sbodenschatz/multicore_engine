/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_reference.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/entity/entity_reference.hpp>

namespace mce {
namespace entity {

entity* entity_reference::resolve() const {
	if(!entity_manager_) return nullptr;
	return entity_manager_->find_entity(referenced_entity_name_);
}

bstream::obstream& operator<<(bstream::obstream& stream, const entity_reference& value) {
	stream << value.referenced_entity_name_;
	return stream;
}
/// Deserializes the entity reference's referenced name, the entity_manager is not deserialized.
bstream::ibstream& operator>>(bstream::ibstream& stream, entity_reference& value) {
	stream >> value.referenced_entity_name_;
	return stream;
}
} // namespace entity
} // namespace mce
