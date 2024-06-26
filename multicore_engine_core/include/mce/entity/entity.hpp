/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/entity.hpp
 * Copyright 2015-2018 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

/**
 * \file
 * Defines the entity class.
 */

#include <boost/container/small_vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/containers/generic_flat_map.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/entity/component.hpp>
#include <mce/entity/component_type_id_manager.hpp>
#include <mce/entity/ecs_types.hpp>

namespace mce {
namespace entity {

class component;
class entity_manager;

/// \brief Represents an entity (aka game object) that is constructed from several component objects following
/// the composition over inheritance technique.
class entity {
private:
	mce::entity::entity_manager& entity_manager_;
	entity_id_t id_;
	entity_position_t position_{0.0f};
	entity_orientation_t orientation_{1.0f, 0.0f, 0.0f, 0.0f};
	template <typename T>
	using component_container = boost::container::small_vector<T, 16>;
	containers::generic_flat_map<component_container, component_type_id_t, component_pool_ptr> components_;
	bool marker_for_despawn = false;

	friend class entity_manager;

public:
	/// Constructs an entity with the given id in the given entity_manager.
	/**
	 * Should only be called in entity_manager but can't be private because it is internally used in an
	 * emplace function.
	 */
	explicit entity(entity_id_t id, entity_manager& em) noexcept : entity_manager_{em}, id_{id} {}
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
	const T* component() const {
		return static_cast<const T*>(component(component_type_id_manager::id<T>()));
	}
	/// \brief Returns the component object of type T associated with this entity if it has such a component
	/// or nullptr otherwise.
	template <typename T>
	T* component() {
		return static_cast<T*>(component(component_type_id_manager::id<T>()));
	}
	/// Checks if the entity has a associated component of type T.
	template <typename T>
	bool has_component() const {
		return has_component(component_type_id_manager::id<T>());
	}

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

	/// \brief Stores the current state of the entity (position, orientation, attached components and their
	/// property values) to the given bstream.
	void store_to_bstream(bstream::obstream& ostr) const;
	/// Loads the state of the entity (as stored by store_to_bstream) from the given bstream.
	/**
	 * The given entity_manager is used to resolve component_types.
	 * The given engine reference is forwarded to component constructors.
	 */
	void load_from_bstream(bstream::ibstream& istr, const mce::entity::entity_manager& ent_mgr,
						   core::engine* engine);

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

	/// \brief Calculates the 4x4 matrix to transform the local coordinate system of the entity to the world
	/// coordinate system.
	glm::mat4 calculate_transform() const {
		glm::mat4 transform = glm::toMat4(orientation_);
		transform[3].x = position_.x;
		transform[3].y = position_.y;
		transform[3].z = position_.z;
		return transform;
	}

	/// Allows access to the entity_manager that contains the entity.
	const mce::entity::entity_manager& entity_manager() const {
		return entity_manager_;
	}
	/// Allows access to the entity_manager that contains the entity.
	mce::entity::entity_manager& entity_manager() {
		return entity_manager_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_HPP_ */
