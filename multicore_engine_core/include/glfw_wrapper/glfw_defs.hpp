/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/glfw_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_GLFW_DEFS_HPP_
#define GLFW_WRAPPER_GLFW_DEFS_HPP_

/**
 * \file
 * Type declarations and enum definitions for the glfw wrapper.
 */

#include <glm/glm.hpp>
#include <util/bit_flags.hpp>
#include <vector>

namespace mce {
namespace glfw {

/// Defines key codes for glfw in a typesafe manner.
enum class key {
	unknown = -1,
	k_space = 32,
	k_apostrophe = 39,
	k_comma = 44,
	k_minus = 45,
	k_period = 46,
	k_slash = 47,
	k_0 = 48,
	k_1 = 49,
	k_2 = 50,
	k_3 = 51,
	k_4 = 52,
	k_5 = 53,
	k_6 = 54,
	k_7 = 55,
	k_8 = 56,
	k_9 = 57,
	k_semicolon = 59,
	k_equal = 61,
	k_a = 65,
	k_b = 66,
	k_c = 67,
	k_d = 68,
	k_e = 69,
	k_f = 70,
	k_g = 71,
	k_h = 72,
	k_i = 73,
	k_j = 74,
	k_k = 75,
	k_l = 76,
	k_m = 77,
	k_n = 78,
	k_o = 79,
	k_p = 80,
	k_q = 81,
	k_r = 82,
	k_s = 83,
	k_t = 84,
	k_u = 85,
	k_v = 86,
	k_w = 87,
	k_x = 88,
	k_y = 89,
	k_z = 90,
	k_left_bracket = 91,
	k_backslash = 92,
	k_right_bracket = 93,
	k_grave_accent = 96,
	k_world_1 = 161,
	k_world_2 = 162,
	k_escape = 256,
	k_enter = 257,
	k_tab = 258,
	k_backspace = 259,
	k_insert = 260,
	k_delete = 261,
	k_right = 262,
	k_left = 263,
	k_down = 264,
	k_up = 265,
	k_page_up = 266,
	k_page_down = 267,
	k_home = 268,
	k_end = 269,
	k_caps_lock = 280,
	k_scroll_lock = 281,
	k_num_lock = 282,
	k_print_screen = 283,
	k_pause = 284,
	k_f1 = 290,
	k_f2 = 291,
	k_f3 = 292,
	k_f4 = 293,
	k_f5 = 294,
	k_f6 = 295,
	k_f7 = 296,
	k_f8 = 297,
	k_f9 = 298,
	k_f10 = 299,
	k_f11 = 300,
	k_f12 = 301,
	k_f13 = 302,
	k_f14 = 303,
	k_f15 = 304,
	k_f16 = 305,
	k_f17 = 306,
	k_f18 = 307,
	k_f19 = 308,
	k_f20 = 309,
	k_f21 = 310,
	k_f22 = 311,
	k_f23 = 312,
	k_f24 = 313,
	k_f25 = 314,
	k_kp_0 = 320,
	k_kp_1 = 321,
	k_kp_2 = 322,
	k_kp_3 = 323,
	k_kp_4 = 324,
	k_kp_5 = 325,
	k_kp_6 = 326,
	k_kp_7 = 327,
	k_kp_8 = 328,
	k_kp_9 = 329,
	k_kp_decimal = 330,
	k_kp_divide = 331,
	k_kp_multiply = 332,
	k_kp_subtract = 333,
	k_kp_add = 334,
	k_kp_enter = 335,
	k_kp_equal = 336,
	k_left_shift = 340,
	k_left_control = 341,
	k_left_alt = 342,
	k_left_super = 343,
	k_right_shift = 344,
	k_right_control = 345,
	k_right_alt = 346,
	k_right_super = 347,
	k_menu = 348,
	last = k_menu
};

/// Describes actions for keys, mouse buttons and joystick buttons in event-handling.
enum class button_action { release = 0, press = 1, repeat = 2 };
/// Defines the bits used to represent the state of modifier keys in event handling.
enum class modifier_bits { shift = 0x0001, control = 0x0002, alt = 0x0004, super = 0x0008 };
/// Describes a combined state of the modifiers represented by the bits in modifier_bits.
typedef util::bit_flags<modifier_bits, 4> modifier_flags;
/// Describes modes in which the (mouse) cursor in glfw can operate.
enum class cursor_mode { normal = 0x00034001, hidden = 0x00034002, disabled = 0x00034003 };

/// Describes possible mouse buttons.
enum class mouse_button {
	button_1 = 0,
	button_2 = 1,
	button_3 = 2,
	button_4 = 3,
	button_5 = 4,
	button_6 = 5,
	button_7 = 6,
	button_8 = 7,
	last = button_8,
	button_left = button_1,
	button_right = button_2,
	button_middle = button_3
};

/// Defines the available bit flags that can be used to influence window behavior.
enum class window_hint_bits {
	resizable = 0x0001,
	visible = 0x0002,
	decorated = 0x0004,
	focused = 0x0008,
	auto_iconify = 0x0010,
	floating = 0x0020,
	maximized = 0x0040
};
/// Describes a window behavior hint state combined from the bits defined in window_hint_bits.
typedef util::bit_flags<window_hint_bits, 7> window_hint_flags;

/// Constant to indicate that glfw should choose the value for certain parameters.
constexpr int dont_care = -1;

/// Describes the parameters of a mode for a monitor.
/**
 * Parameters that are indicated to allow dont_care can be set to dont_care if this struct is supplied from
 * user code on window creation, but can not be a result when listing monitor modes.
 */
struct video_mode {
	int width;		  ///< Horizontal size in virtual screen coordinates.
	int height;		  ///< Vertical size in virtual screen coordinates.
	int red_bits;	 ///< Red channel bit depth, can be dont_care.
	int green_bits;   ///< Green channel bit depth, can be dont_care.
	int blue_bits;	///< Blue channel bit depth, can be dont_care.
	int refresh_rate; ///< Refresh rate in Hz, can be dont_care.
	video_mode() : video_mode(0, 0) {}
	/// Creates a video_mode with the given parameters.
	video_mode(int width, int height, int redBits = dont_care, int greenBits = dont_care,
			   int blueBits = dont_care, int refreshRate = dont_care)
			: width{width}, height{height}, red_bits{redBits}, green_bits{greenBits}, blue_bits{blueBits},
			  refresh_rate{refreshRate} {}
};

/// Describes gamma curve, discretized to rgb points, should consist of 256 points to be portable.
typedef std::vector<glm::tvec3<unsigned short>> gamma_ramp;

/// Describes possible events for devices reflected in callbacks for monitor and joystick config changes.
enum class device_event { connected = 0x00040001, disconnected = 0x00040002 };

/// Describes the available standard cursors.
enum class standard_cursor {
	arrow = 0x00036001,
	ibeam = 0x00036002,
	crosshair = 0x00036003,
	hand = 0x00036004,
	h_resize = 0x00036005,
	v_resize = 0x00036006
};

/// Describes the error codes used in error callbacks.
enum class error_code {
	not_initialized = 0x00010001,
	no_current_context = 0x00010002,
	invalid_enum = 0x00010003,
	invalid_value = 0x00010004,
	out_of_memory = 0x00010005,
	api_unavailable = 0x00010006,
	version_unavailable = 0x00010007,
	platform_error = 0x00010008,
	format_unavailable = 0x00010009,
	no_window_context = 0x0001000A
};

} // namespace glfw_wrapper
} // namespace mce

#endif /* GLFW_WRAPPER_GLFW_DEFS_HPP_ */
