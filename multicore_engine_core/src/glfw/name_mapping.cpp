/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw/name_mapping.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/container/flat_map.hpp>
#include <mce/glfw/name_mapping.hpp>
#include <mce/util/map_utils.hpp>

namespace mce {
namespace glfw {

using namespace std::literals;

#define KEY_MAP_ENTRY(K)                                                                                     \
	{ key::k_##K, #K }

static const boost::container::flat_map<key, std::string> key_to_name_mapping = {
		{KEY_MAP_ENTRY(0),
		 KEY_MAP_ENTRY(1),
		 KEY_MAP_ENTRY(2),
		 KEY_MAP_ENTRY(3),
		 KEY_MAP_ENTRY(4),
		 KEY_MAP_ENTRY(5),
		 KEY_MAP_ENTRY(6),
		 KEY_MAP_ENTRY(7),
		 KEY_MAP_ENTRY(8),
		 KEY_MAP_ENTRY(9),
		 KEY_MAP_ENTRY(space),
		 KEY_MAP_ENTRY(apostrophe),
		 KEY_MAP_ENTRY(comma),
		 KEY_MAP_ENTRY(minus),
		 KEY_MAP_ENTRY(period),
		 KEY_MAP_ENTRY(slash),
		 KEY_MAP_ENTRY(semicolon),
		 KEY_MAP_ENTRY(equal),
		 KEY_MAP_ENTRY(a),
		 KEY_MAP_ENTRY(b),
		 KEY_MAP_ENTRY(c),
		 KEY_MAP_ENTRY(d),
		 KEY_MAP_ENTRY(e),
		 KEY_MAP_ENTRY(f),
		 KEY_MAP_ENTRY(g),
		 KEY_MAP_ENTRY(h),
		 KEY_MAP_ENTRY(i),
		 KEY_MAP_ENTRY(j),
		 KEY_MAP_ENTRY(k),
		 KEY_MAP_ENTRY(l),
		 KEY_MAP_ENTRY(m),
		 KEY_MAP_ENTRY(n),
		 KEY_MAP_ENTRY(o),
		 KEY_MAP_ENTRY(p),
		 KEY_MAP_ENTRY(q),
		 KEY_MAP_ENTRY(r),
		 KEY_MAP_ENTRY(s),
		 KEY_MAP_ENTRY(t),
		 KEY_MAP_ENTRY(u),
		 KEY_MAP_ENTRY(v),
		 KEY_MAP_ENTRY(w),
		 KEY_MAP_ENTRY(x),
		 KEY_MAP_ENTRY(y),
		 KEY_MAP_ENTRY(z),
		 KEY_MAP_ENTRY(left_bracket),
		 KEY_MAP_ENTRY(backslash),
		 KEY_MAP_ENTRY(right_bracket),
		 KEY_MAP_ENTRY(grave_accent),
		 KEY_MAP_ENTRY(world_1),
		 KEY_MAP_ENTRY(world_2),
		 KEY_MAP_ENTRY(escape),
		 KEY_MAP_ENTRY(enter),
		 KEY_MAP_ENTRY(tab),
		 KEY_MAP_ENTRY(backspace),
		 KEY_MAP_ENTRY(insert),
		 KEY_MAP_ENTRY(delete),
		 KEY_MAP_ENTRY(right),
		 KEY_MAP_ENTRY(left),
		 KEY_MAP_ENTRY(down),
		 KEY_MAP_ENTRY(up),
		 KEY_MAP_ENTRY(page_up),
		 KEY_MAP_ENTRY(page_down),
		 KEY_MAP_ENTRY(home),
		 KEY_MAP_ENTRY(end),
		 KEY_MAP_ENTRY(caps_lock),
		 KEY_MAP_ENTRY(scroll_lock),
		 KEY_MAP_ENTRY(num_lock),
		 KEY_MAP_ENTRY(print_screen),
		 KEY_MAP_ENTRY(pause),
		 KEY_MAP_ENTRY(f1),
		 KEY_MAP_ENTRY(f2),
		 KEY_MAP_ENTRY(f3),
		 KEY_MAP_ENTRY(f4),
		 KEY_MAP_ENTRY(f5),
		 KEY_MAP_ENTRY(f6),
		 KEY_MAP_ENTRY(f7),
		 KEY_MAP_ENTRY(f8),
		 KEY_MAP_ENTRY(f9),
		 KEY_MAP_ENTRY(f10),
		 KEY_MAP_ENTRY(f11),
		 KEY_MAP_ENTRY(f12),
		 KEY_MAP_ENTRY(f13),
		 KEY_MAP_ENTRY(f14),
		 KEY_MAP_ENTRY(f15),
		 KEY_MAP_ENTRY(f16),
		 KEY_MAP_ENTRY(f17),
		 KEY_MAP_ENTRY(f18),
		 KEY_MAP_ENTRY(f19),
		 KEY_MAP_ENTRY(f20),
		 KEY_MAP_ENTRY(f21),
		 KEY_MAP_ENTRY(f22),
		 KEY_MAP_ENTRY(f23),
		 KEY_MAP_ENTRY(f24),
		 KEY_MAP_ENTRY(f25),
		 KEY_MAP_ENTRY(kp_0),
		 KEY_MAP_ENTRY(kp_1),
		 KEY_MAP_ENTRY(kp_2),
		 KEY_MAP_ENTRY(kp_3),
		 KEY_MAP_ENTRY(kp_4),
		 KEY_MAP_ENTRY(kp_5),
		 KEY_MAP_ENTRY(kp_6),
		 KEY_MAP_ENTRY(kp_7),
		 KEY_MAP_ENTRY(kp_8),
		 KEY_MAP_ENTRY(kp_9),
		 KEY_MAP_ENTRY(kp_decimal),
		 KEY_MAP_ENTRY(kp_divide),
		 KEY_MAP_ENTRY(kp_multiply),
		 KEY_MAP_ENTRY(kp_subtract),
		 KEY_MAP_ENTRY(kp_add),
		 KEY_MAP_ENTRY(kp_enter),
		 KEY_MAP_ENTRY(kp_equal),
		 KEY_MAP_ENTRY(left_shift),
		 KEY_MAP_ENTRY(left_control),
		 KEY_MAP_ENTRY(left_alt),
		 KEY_MAP_ENTRY(left_super),
		 KEY_MAP_ENTRY(right_shift),
		 KEY_MAP_ENTRY(right_control),
		 KEY_MAP_ENTRY(right_alt),
		 KEY_MAP_ENTRY(right_super),
		 KEY_MAP_ENTRY(menu)}};
static const boost::container::flat_map<std::string, key> name_to_key_mapping =
		util::inverse_map(key_to_name_mapping);
static const std::string unknown_key_name = "unknown";

#define MOUSE_BUTTON_MAP_ENTRY(MB)                                                                           \
	{ mouse_button::button_##MB, #MB }

static const boost::container::flat_map<mouse_button, std::string> mouse_button_to_name_mapping = {
		{MOUSE_BUTTON_MAP_ENTRY(1), MOUSE_BUTTON_MAP_ENTRY(2), MOUSE_BUTTON_MAP_ENTRY(3),
		 MOUSE_BUTTON_MAP_ENTRY(4), MOUSE_BUTTON_MAP_ENTRY(5), MOUSE_BUTTON_MAP_ENTRY(6),
		 MOUSE_BUTTON_MAP_ENTRY(7), MOUSE_BUTTON_MAP_ENTRY(8), MOUSE_BUTTON_MAP_ENTRY(left),
		 MOUSE_BUTTON_MAP_ENTRY(right), MOUSE_BUTTON_MAP_ENTRY(middle)}};
static const boost::container::flat_map<std::string, mouse_button> name_to_mouse_button_mapping =
		util::inverse_map(mouse_button_to_name_mapping);
static const std::string unknown_mouse_button_name = "";

const std::string& to_string(key k) {
	auto it = key_to_name_mapping.find(k);
	if(it == key_to_name_mapping.end()) return unknown_key_name;
	return it->second;
}

key key_from_string(const std::string& name) {
	auto it = name_to_key_mapping.find(name);
	if(it == name_to_key_mapping.end()) return key::unknown;
	return it->second;
}

const std::string& to_string(mouse_button b) {
	auto it = mouse_button_to_name_mapping.find(b);
	if(it == mouse_button_to_name_mapping.end()) return unknown_mouse_button_name;
	return it->second;
}
boost::optional<mouse_button> mouse_button_from_string(const std::string& name) {
	auto it = name_to_mouse_button_mapping.find(name);
	if(it == name_to_mouse_button_mapping.end()) return {};
	return it->second;
}

} /* namespace glfw */
} /* namespace mce */
