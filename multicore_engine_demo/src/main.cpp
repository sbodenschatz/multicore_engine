/*
 * Multi-Core Engine project
 * File /mutlicore_engine_demo/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <array>
#include <mce/core/engine.hpp>
#include <mce/glfw/window.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/simple_descriptor_pool.hpp>
#include <mce/graphics/unique_descriptor_pool.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/array_utils.hpp>
#include <vulkan/vulkan.hpp>

int main() {
	mce::graphics::instance ai;
	mce::graphics::device dev(ai);
	mce::glfw::window w("Test", {800, 600});
	mce::graphics::window win(ai, w, dev);
	mce::graphics::device_memory_manager mm(&dev, 1u << 26);
	mce::graphics::pipeline_cache pc(dev);

	mce::graphics::simple_descriptor_pool dp(dev, 128, {{vk::DescriptorType::eUniformBuffer, 128}});
	auto sets = dp.allocate_descriptor_sets(
			mce::util::make_array(std::shared_ptr<mce::graphics::descriptor_set_layout>()));
	mce::graphics::unique_descriptor_pool dp2(dev, 128, {{vk::DescriptorType::eUniformBuffer, 128}});
	auto sets2 = dp2.allocate_descriptor_sets(
			mce::util::make_array(std::shared_ptr<mce::graphics::descriptor_set_layout>()));
}
