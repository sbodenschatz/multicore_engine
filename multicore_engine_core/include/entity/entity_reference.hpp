/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_reference.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_REFERENCE_HPP_
#define ENTITY_ENTITY_REFERENCE_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <string>

namespace mce {
namespace bstream {
class ibstream;
class obstream;
} // namespace bstream
namespace entity {
class entity_manager;
class entity;

/// Represents a reference to an entity by it's name that can be resolved to the actual entity.
class entity_reference {
private:
	std::string referenced_entity_name_;
	const mce::entity::entity_manager* entity_manager_;

public:
	/// Creates an entity_reference referring to the entity with the given name in the given entity_manager.
	// cppcheck-suppress passedByValue
	entity_reference(std::string referenced_entity_name, const entity_manager& entity_manager)
			: referenced_entity_name_{std::move(referenced_entity_name)}, entity_manager_{&entity_manager} {}

	/// Creates an empty entity_reference.
	entity_reference() : referenced_entity_name_{""}, entity_manager_{nullptr} {}

	/// \brief Returns the referenced entity or nullptr if the entity does not exist or the entity_manager
	/// reference is empty.
	entity* resolve() const;

	/// Returns the entity_manager that will be used to resolve the name to an entity.
	const mce::entity::entity_manager* entity_manager() const {
		return entity_manager_;
	}

	/// Returns the name of the referenced entity.
	const std::string& referenced_entity_name() const {
		return referenced_entity_name_;
	}

	friend bstream::obstream& operator<<(bstream::obstream& stream, const entity_reference& value);
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, entity_reference& value);
};

/// Serializes the entity_reference by saving the referenced name.
bstream::obstream& operator<<(bstream::obstream& stream, const entity_reference& value);
/// Deserializes the entity reference's referenced name, the entity_manager is not deserialized.
bstream::ibstream& operator>>(bstream::ibstream& stream, entity_reference& value);

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_REFERENCE_HPP_ */
