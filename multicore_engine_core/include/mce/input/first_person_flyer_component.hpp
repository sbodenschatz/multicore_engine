/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/input/first_person_flyer_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_
#define MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_

#include <mce/entity/component.hpp>

namespace mce {
namespace input {

class first_person_flyer_component : public entity::component {
public:
	first_person_flyer_component(entity::entity& owner, const entity::component_configuration& configuration);
	~first_person_flyer_component();
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_ */
