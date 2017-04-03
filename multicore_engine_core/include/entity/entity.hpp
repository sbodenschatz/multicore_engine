/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

#include "../containers/generic_flat_map.hpp"
#include "../containers/smart_pool_ptr.hpp"
#include "ecs_types.hpp"
#include <boost/container/small_vector.hpp>

namespace mce {
namespace entity {

class component;

/// \brief Represents an entity (aka game object) that is constructed from several component objects following
/// the composition over inheritance technique.
class entity {
private:
	entity_id_t id_;
	entity_position_t position_;
	entity_orientation_t orientation_;
	template <typename T>
	using component_container = boost::container::small_vector<T, 16>;
	containers::generic_flat_map<component_container, component_type_id_t,
								 containers::smart_pool_ptr<mce::entity::component>>
			components_;
	bool marker_for_despawn = false;

public:
	/// Constructs an entity with the given id.
	explicit entity(entity_id_t id) noexcept : id_(id) {}
	/// Forbids copy-construction for entity.
	entity(const entity&) = delete;
	/// Forbids move-construction for entity.
	entity(entity&&) = delete;
	/// Forbids copy-assignment for entity.
	entity& operator=(const entity&) = delete;
	/// Forbids move-assignment for entity.
	entity& operator=(entity&&) = delete;
	/// Destroys the entity.
	~entity() = default;

	/// \brief Returns the component object of type T associated with this entity if it has such a component
	/// or nullptr otherwise.
	template <typename T>
	const mce::entity::component* component() const;
	/// \brief Returns the component object of type T associated with this entity if it has such a component
	/// or nullptr otherwise.
	template <typename T>
	mce::entity::component* component();
	/// Checks if the entity has a associated component of type T.
	template <typename T>
	bool has_component() const;

	/// \brief Returns the component object with the given type id associated with this entity if it has such
	/// a component or nullptr otherwise.
	const mce::entity::component* component(component_type_id_t id) const;
	/// \brief Returns the component object with the given type id associated with this entity if it has such
	/// a component or nullptr otherwise.
	mce::entity::component* component(component_type_id_t id);
	/// Checks if the entity has a associated component with the given type id.
	bool has_component(component_type_id_t id) const;

	/// Adds the given component to the entity.
	void add_component(component_pool_ptr&& comp);
	/// Allows access to the container containing the associated component objects.
	const containers::generic_flat_map<component_container, component_type_id_t, component_pool_ptr>&
	components() const {
		return components_;
	}
	/// Returns the id of the entity.
	entity_id_t id() const {
		return id_;
	}
	/// Returns the orientation of the entity.
	const entity_orientation_t& orientation() const {
		return orientation_;
	}
	/// Sets the orientation of the entity to the given value.
	void orientation(const entity_orientation_t& orientation) {
		orientation_ = orientation;
	}
	/// Returns the position of the entity.
	const entity_position_t& position() const {
		return position_;
	}
	/// Sets the position of the entity to the given value.
	void position(const entity_position_t& position) {
		position_ = position;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_HPP_ */
