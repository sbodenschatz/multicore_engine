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

void texture::complete_loading(const asset::asset_ptr& tex_asset) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	auto tex = gli::load_dds(tex_asset->data(), tex_asset->size());
	base_image* bimg = nullptr;
	if(tex.faces() == 1 && tex.layers() == 1) {
		auto format = vk::Format::eUndefined; // TODO Determine format from loaded texture.
		image_2d img(mgr_.dev_, mgr_.mem_mgr_, mgr_.destruction_manager_, format,
					 image_2d::size_type(tex.extent()), uint32_t(tex.levels()),
					 vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);
		image_view_ = img.create_view();
		image_ = std::move(img);
		bimg = &boost::strict_get<image_2d>(image_);
	} else {
		raise_error_flag(std::make_exception_ptr(mce::graphics_exception("Unsupported exception type.")));
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
