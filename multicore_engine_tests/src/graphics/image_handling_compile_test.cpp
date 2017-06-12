/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/image_handling_compile_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/application_instance.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/image.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

static void test_image_handling_compilation() {
	mce::graphics::application_instance ai;
	mce::graphics::device dev(ai);
	mce::glfw::window w("Test", {800, 600});
	mce::graphics::window win(ai, w, dev);
	mce::graphics::device_memory_manager mm(&dev, 1u << 26);
	mce::graphics::image_2d img(dev, mm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024},
								vk::ImageUsageFlagBits::eSampled, vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv = img.create_view();
	mce::graphics::image_1d img2(dev, mm, vk::Format::eA8B8G8R8UnormPack32, 1024,
								 vk::ImageUsageFlagBits::eSampled, vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv2 = img2.create_view();
	mce::graphics::image_3d img3(dev, mm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024, 1024},
								 vk::ImageUsageFlagBits::eSampled, vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv3 = img3.create_view();

	mce::graphics::image_2d_layered img4(dev, mm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024},
										 vk::ImageUsageFlagBits::eSampled, 8,
										 vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv4 = img4.create_view();
	auto iv4_1 = img4.create_single_layer_view(2);
	mce::graphics::image_1d_layered img5(dev, mm, vk::Format::eA8B8G8R8UnormPack32, 1024,
										 vk::ImageUsageFlagBits::eSampled, 8,
										 vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv5 = img5.create_view();
	auto iv5_1 = img5.create_single_layer_view(2);
	mce::graphics::image_cube img6(dev, mm, vk::Format::eA8B8G8R8UnormPack32, 1024,
								   vk::ImageUsageFlagBits::eSampled, vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv6 = img6.create_view();
	auto iv6_1 = img6.create_single_side_view(3);
	auto iv6_2 = img6.create_layered_side_view(4);
	mce::graphics::image_cube_layered img7(dev, mm, vk::Format::eA8B8G8R8UnormPack32, 1024,
										   vk::ImageUsageFlagBits::eSampled, 8,
										   vk::ImageLayout::eShaderReadOnlyOptimal);
	auto iv7 = img7.create_view();
	auto iv7_1 = img7.create_single_layer_view(2);
	auto iv7_2 = img7.create_single_side_view(3);
	auto iv7_3 = img7.create_layered_side_view(4);
}

TEST(graphics_image_handling, compile_test) {
	UNUSED(&test_image_handling_compilation);
	ASSERT_TRUE(true);
}

} // namespace graphics
} // namespace mce
