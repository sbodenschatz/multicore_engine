/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/texture.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4310)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
#endif
#include <gli/gli.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <mce/asset/asset.hpp>
#include <mce/graphics/texture.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <mce/graphics/transfer_manager.hpp>

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
	image_ = T(mgr_.dev_, mgr_.mem_mgr_, mgr_.destruction_manager_, format,
			   typename T::size_type(tex.extent()), uint32_t(tex.levels()),
			   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
	image_view_ = boost::strict_get<T>(image_).create_view();
	img_handle = boost::strict_get<T>(image_).native_image();
	img_view_handle = boost::strict_get<decltype(boost::strict_get<T>(image_).create_view())>(image_view_)
							  .native_view();
	return &boost::strict_get<T>(image_);
}
template <typename T>
base_image* texture::create_image_layered(vk::Format format, const gli::texture& tex) {
	image_ = T(mgr_.dev_, mgr_.mem_mgr_, mgr_.destruction_manager_, format,
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
	auto tex = gli::load_dds(tex_asset->data(), tex_asset->size());
	base_image* bimg = nullptr;
	auto format = vk::Format::eUndefined; // TODO Determine format from loaded texture.
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
	// TODO Fill regions
	mgr_.transfer_mgr_.upload_image(tex.data(), tex.size(), *bimg, vk::ImageLayout::eShaderReadOnlyOptimal,
									regions, [this_shared](vk::Image) { this_shared->complete_staging(); });
}

} /* namespace graphics */
} /* namespace mce */
