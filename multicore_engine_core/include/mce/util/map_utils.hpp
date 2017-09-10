/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/map_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_MAP_UTILS_HPP_
#define MCE_UTIL_MAP_UTILS_HPP_

/**
 * \file
 * Defines utility functions for working with maps.
 */

#include <boost/container/flat_map.hpp>

namespace mce {
namespace util {

/// Takes a map A->B and generates a map B->A from it.
template <typename Key, typename Val>
boost::container::flat_map<Val, Key> inverse_map(const boost::container::flat_map<Key, Val>& map) {
	boost::container::flat_map<Val, Key> res;
	res.reserve(map.size());
	for(const auto& elem : map) {
		res.emplace(elem.second, elem.first);
	}
	return res;
}

/// Returns the keys of the given map in a container of type Res, that must support reserve and push_back.
template <typename Res, typename In>
Res map_keys(const In& map) {
	Res res;
	res.reserve(map.size());
	for(const auto& elem : map) {
		res.push_back(elem.first);
	}
	return res;
}

/// Returns the values of the given map in a container of type Res, that must support reserve and push_back.
template <typename Res, typename In>
Res map_values(const In& map) {
	Res res;
	res.reserve(map.size());
	for(const auto& elem : map) {
		res.push_back(elem.second);
	}
	return res;
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_MAP_UTILS_HPP_ */
