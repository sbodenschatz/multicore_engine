/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/graphics/transfer_manager_compile_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/image.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

static void test_transfer_manager_compilation() {
	mce::graphics::instance ai;
	mce::graphics::device dev(ai);
	mce::glfw::window w("Test", {800, 600});
	mce::graphics::window win(ai, w, dev);
	mce::graphics::device_memory_manager mm(&dev, 1u << 26);
	mce::graphics::destruction_queue_manager dqm(&dev, 3);
	mce::graphics::image_2d img(dev, mm, &dqm, vk::Format::eA8B8G8R8UnormPack32, {1024, 1024},
								mce::graphics::image_2d::full_mip_levels(glm::vec2{1024, 1024}),
								vk::ImageUsageFlagBits::eSampled);
	mce::graphics::transfer_manager tm(dev, mm, 3);
	tm.upload_image(std::shared_ptr<const char>(), 1 << 20, img, vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})});
	tm.upload_image(std::shared_ptr<const char>(), 1 << 20, img, vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})},
					[](vk::Image) {});
	tm.upload_image(containers::pooled_byte_buffer_ptr{}, 1 << 20, img,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})});
	tm.upload_image(containers::pooled_byte_buffer_ptr{}, 1 << 20, img,
					vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})},
					[](vk::Image) {});
	void* data_ptr = nullptr;
	tm.upload_image(data_ptr, 1 << 20, img, vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})});
	tm.upload_image(data_ptr, 1 << 20, img, vk::ImageLayout::eShaderReadOnlyOptimal,
					{vk::BufferImageCopy(0, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
										 {0, 0, 0}, {1024, 1024, 1})},
					[](vk::Image) {});
	mce::graphics::buffer buff(dev, mm, &dqm, 1024, vk::BufferUsageFlagBits::eVertexBuffer);
	tm.upload_buffer(std::shared_ptr<const char>(), 1024, buff.native_buffer(), 0, [](vk::Buffer) {});
	tm.upload_buffer(containers::pooled_byte_buffer_ptr{}, 1024, buff.native_buffer(), 0, [](vk::Buffer) {});
	tm.upload_buffer(data_ptr, 1024, buff.native_buffer(), 0, [](vk::Buffer) {});
	tm.upload_buffer(std::shared_ptr<const char>(), 1024, buff.native_buffer(), 0);
	tm.upload_buffer(containers::pooled_byte_buffer_ptr{}, 1024, buff.native_buffer(), 0);
	tm.upload_buffer(data_ptr, 1024, buff.native_buffer(), 0);
}

TEST(graphics_transfer_manager, compile_test) {
	UNUSED(&test_transfer_manager_compilation);
	ASSERT_TRUE(true);
}

} // namespace graphics
} // namespace mce
