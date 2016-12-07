/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/model_loader.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_MANAGER_HPP_
#define MODEL_MODEL_MANAGER_HPP_

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset

namespace model {

class model_manager {
public:
	model_manager(asset::asset_manager& asset_manager) noexcept : asset_manager(asset_manager) {}

private:
	asset::asset_manager& asset_manager;
};

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_MANAGER_HPP_ */
