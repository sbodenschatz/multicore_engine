/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/vk_mock_interface_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/graphics/vk_mock_interface.hpp>

namespace mce {
namespace graphics {

TEST(graphics, mock_symbols_overridden) {
	ASSERT_TRUE(vk_mock_interface::is_mocked());
}

} // namespace graphics
} // namespace mce
