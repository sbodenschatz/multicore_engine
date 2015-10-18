/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_HPP_
#define ENTITY_ENTITY_HPP_

#include <boost/container/small_vector.hpp>
#include "../containers/generic_flat_map.hpp"
#include "../containers/smart_pool_ptr.hpp"
#include "ecs_types.hpp"

namespace mce {
namespace entity {

class component;

class entity {
private:
	entity_id_t id_;
	entity_position_t position_;
	entity_orientation_t orientation_;
	template <typename T>
	using component_container = boost::container::small_vector<T, 16>;
	containers::generic_flat_map<component_container, component_type_id_t,
								 containers::smart_pool_ptr<mce::entity::component>> components_;
	entity(entity_id_t id) noexcept : id_(id) {}

public:
	entity(const entity&) = delete;
	entity(entity&&) = delete;
	entity& operator=(const entity&) = delete;
	entity& operator=(entity&&) = delete;
	~entity() = default;

	template <typename T>
	const mce::entity::component* component() const;
	template <typename T>
	mce::entity::component* component();
	template <typename T>
	bool has_component() const;

	const mce::entity::component* component(component_type_id_t id) const;
	mce::entity::component* component(component_type_id_t id);
	bool has_component(component_type_id_t id) const;
	void add_component(component_pool_ptr&& comp);

	const containers::generic_flat_map<component_container, component_type_id_t, component_pool_ptr>&
	components() const {
		return components_;
	}

	entity_id_t id() const {
		return id_;
	}

	const entity_orientation_t& orientation() const {
		return orientation_;
	}

	void orientation(const entity_orientation_t& orientation) {
		orientation_ = orientation;
	}

	const entity_position_t& position() const {
		return position_;
	}

	void position(const entity_position_t& position) {
		position_ = position;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_HPP_ */
