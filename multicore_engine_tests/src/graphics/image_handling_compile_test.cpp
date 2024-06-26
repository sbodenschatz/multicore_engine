/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/image_handling_compile_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

// TODO: Find source of libc++ linking problem with the compile tests
#ifndef MCECLANG

#include <gtest.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/image.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

static void test_image_handling_compilation() {
	mce::graphics::instance ai;
	mce::graphics::device dev(ai);
	mce::glfw::window w("Test", {800, 600});
	mce::graphics::window win(ai, w, dev);
	mce::graphics::device_memory_manager mm(&dev, 1u << 26);
	mce::graphics::destruction_queue_manager dqm(&dev, 3);
	mce::graphics::image_2d img(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024},
								mce::graphics::image_2d::full_mip_levels(glm::vec2{1024, 1024}),
								vk::ImageUsageFlagBits::eSampled);
	auto iv = img.create_view();
	mce::graphics::image_1d img2(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, 1024, 1,
								 vk::ImageUsageFlagBits::eSampled);
	auto iv2 = img2.create_view();
	mce::graphics::image_3d img3(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024, 1024}, 1,
								 vk::ImageUsageFlagBits::eSampled);
	auto iv3 = img3.create_view();

	mce::graphics::image_cube img6(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, 1024, 1,
								   vk::ImageUsageFlagBits::eSampled);
	auto iv6 = img6.create_view();
	auto iv6_1 = img6.create_face_view(3);
	auto iv6_2 = img6.create_faces_view();

	mce::graphics::image_2d_layered img4(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024, 8}, 1,
										 vk::ImageUsageFlagBits::eSampled);
	auto iv4 = img4.create_view();
	auto iv4_1 = img4.create_single_view(2);
	mce::graphics::image_1d_layered img5(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 8}, 1,
										 vk::ImageUsageFlagBits::eSampled);
	auto iv5 = img5.create_view();
	auto iv5_1 = img5.create_single_view(2);
	mce::graphics::image_cube_layered img7(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 8 * 6}, 1,
										   vk::ImageUsageFlagBits::eSampled);
	auto iv7 = img7.create_view();
	auto iv7_1 = img7.create_single_view(2);
	auto iv7_2 = img7.create_face_view(3);
	auto iv7_3 = img7.create_faces_view(6, 12);
}

TEST(graphics_image_handling, compile_test) {
	UNUSED(&test_image_handling_compilation);
	ASSERT_TRUE(true);
}

} // namespace graphics
} // namespace mce

#endif
