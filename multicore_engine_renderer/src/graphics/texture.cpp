/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/texture.cpp
 * Copyright 2017-2018 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4701)
#endif
#include <glm/gtc/type_precision.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4310)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
#pragma warning(disable : 4005)
#pragma warning(disable : 4701)
#endif
#include <gli/gli.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <mce/asset/asset.hpp>
#include <mce/graphics/sampler.hpp>
#include <mce/graphics/texture.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <vulkan/vk_format.h>

namespace mce {
namespace graphics {

texture::~texture() {}

void texture::raise_error_flag(std::exception_ptr e) noexcept {
	current_state_ = state::error;
	std::unique_lock<std::mutex> lock(modification_mutex);
	for(auto& handler : error_handlers) {
		try {
			handler(e);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	error_handlers.clear();
	completion_handlers.clear();
	error_handlers.shrink_to_fit();
	completion_handlers.shrink_to_fit();
}

template <typename T>
base_image* texture::create_image(vk::Format format, const gli::texture& tex) {
	auto md = mgr_deps.lock();
	if(!md) throw mce::async_state_exception("Manager object expired when required by callback.");
	image_ = T(md->dev_, md->mem_mgr_, md->destruction_manager_, format, typename T::size_type(tex.extent()),
			   uint32_t(tex.levels()),
			   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	image_view_ = boost::strict_get<T>(image_).create_view();
	img_handle = boost::strict_get<T>(image_).native_image();
	img_view_handle = boost::strict_get<decltype(boost::strict_get<T>(image_).create_view())>(image_view_)
							  .native_view();
	return &boost::strict_get<T>(image_);
}
template <typename T>
base_image* texture::create_image_layered(vk::Format format, const gli::texture& tex) {
	auto md = mgr_deps.lock();
	if(!md) throw mce::async_state_exception("Manager object expired when required by callback.");
	image_ = T(md->dev_, md->mem_mgr_, md->destruction_manager_, format,
			   typename T::size_type(tex.extent(), uint32_t(tex.layers())), uint32_t(tex.levels()),
			   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	image_view_ = boost::strict_get<T>(image_).create_view();
	img_handle = boost::strict_get<T>(image_).native_image();
	img_view_handle = boost::strict_get<decltype(boost::strict_get<T>(image_).create_view())>(image_view_)
							  .native_view();
	return &boost::strict_get<T>(image_);
}

void texture::complete_loading(const asset::asset_ptr& tex_asset) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	try {
		auto tex = gli::load_dds(tex_asset->data(), tex_asset->size());
		base_image* bimg = nullptr;
		auto gl_format = gli::gl(gli::gl::PROFILE_GL33).translate(tex.format(), tex.swizzles());
		auto format = static_cast<vk::Format>(vkGetFormatFromOpenGLInternalFormat(gl_format.Internal));
		switch(tex.target()) {
		case gli::TARGET_1D: bimg = create_image<image_1d>(format, tex); break;
		case gli::TARGET_2D: bimg = create_image<image_2d>(format, tex); break;
		case gli::TARGET_3D: bimg = create_image<image_3d>(format, tex); break;
		case gli::TARGET_CUBE: bimg = create_image<image_cube>(format, tex); break;
		case gli::TARGET_1D_ARRAY: bimg = create_image_layered<image_1d_layered>(format, tex); break;
		case gli::TARGET_2D_ARRAY: bimg = create_image_layered<image_2d_layered>(format, tex); break;
		case gli::TARGET_CUBE_ARRAY: bimg = create_image_layered<image_cube_layered>(format, tex); break;
		default:
			raise_error_flag(std::make_exception_ptr(mce::graphics_exception("Unsupported texture type.")));
			break;
		}

		auto this_shared = this->shared_from_this();
		current_state_ = state::staging;
		lock.unlock();
		std::vector<vk::BufferImageCopy> regions;
		auto base = static_cast<const char*>(tex.data());
		for(std::size_t layer = 0; layer < tex.layers(); ++layer) {
			for(std::size_t face = 0; face < tex.faces(); ++face) {
				for(std::size_t level = 0; level < tex.levels(); ++level) {
					vk::DeviceSize offset = static_cast<const char*>(tex.data(layer, face, level)) - base;
					auto ext = tex.extent(level);
					regions.emplace_back(offset, 0, 0,
										 vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
																	uint32_t(level),
																	uint32_t(layer * tex.faces() + face), 1),
										 vk::Offset3D(0, 0, 0), vk::Extent3D(ext.x, ext.y, ext.z));
				}
			}
		}
		auto md = mgr_deps.lock();
		if(md) {
			md->transfer_mgr_.upload_image(tex.data(), tex.size(), *bimg,
										   vk::ImageLayout::eShaderReadOnlyOptimal, regions,
										   [this_shared](vk::Image) { this_shared->complete_staging(); });
		} else {
			raise_error_flag(std::make_exception_ptr(
					mce::async_state_exception("Manager object expired when required by callback.")));
		}
	} catch(...) {
		raise_error_flag(std::current_exception());
	}
}

void texture::complete_staging() noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const texture>(this->shared_from_this());
	lock.unlock();
	// From here on the texture object is immutable and can therefore be read without holding a lock
	for(auto& handler : completion_handlers) {
		try {
			handler(this_shared);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	completion_handlers.clear();
	error_handlers.clear();
	completion_handlers.shrink_to_fit();
	error_handlers.shrink_to_fit();
}

vk::DescriptorImageInfo texture::bind(const sampler* sampler_to_use) const {
	vk::Sampler sampler_handle;
	if(sampler_to_use) {
		sampler_handle = sampler_to_use->native_sampler();
	}
	return vk::DescriptorImageInfo(sampler_handle, img_view_handle, vk::ImageLayout::eShaderReadOnlyOptimal);
}

} /* namespace graphics */
} /* namespace mce */
