/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/pipeline_config.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_CONFIG_HPP_
#define GRAPHICS_PIPELINE_CONFIG_HPP_

/**
 * \file
 * Provides the pipeline_config class that contains all data required for creating a graphics pipeline.
 */

#include <algorithm>
#include <boost/optional.hpp>
#include <mce/graphics/render_pass.hpp>
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class shader_module;
class pipeline_layout;

/// Bundles and owns all data required for creating a pipeline.
class pipeline_config {
public:
	/// Bundles all data for a shader stage.
	class shader_stage_config {
	private:
		vk::ShaderStageFlagBits stage_;
		std::shared_ptr<const shader_module> module_;
		std::string entry_point_name_;
		std::vector<char> specialization_data_;
		std::vector<vk::SpecializationMapEntry> specialization_map_;
		vk::SpecializationInfo specialization_info_;

		void update_specialization_info() {
			specialization_info_.dataSize = specialization_data_.size();
			specialization_info_.pData = specialization_data_.data();
			specialization_info_.mapEntryCount = uint32_t(specialization_map_.size());
			specialization_info_.pMapEntries = specialization_map_.data();
		}

	public:
		/// Creates an empty shader_stage_config.
		shader_stage_config() : stage_{vk::ShaderStageFlagBits::eVertex} {}
		/// Creates a shader_stage_config from the given parameters.
		shader_stage_config(vk::ShaderStageFlagBits stage, std::shared_ptr<const shader_module> module,
							std::string entry_point_name, std::vector<char> specialization_data = {},
							std::vector<vk::SpecializationMapEntry> specialization_map = {})
				: stage_{stage}, module_{std::move(module)}, entry_point_name_{std::move(entry_point_name)},
				  specialization_data_{std::move(specialization_data)}, specialization_map_{std::move(
																				specialization_map)} {}

		/// Gets the name of the entry point.
		const std::string& entry_point_name() const {
			return entry_point_name_;
		}

		/// Sets the name of the entry point.
		void entry_point_name(const std::string& entry_point_name) {
			entry_point_name_ = entry_point_name;
		}

		/// Gets the shader module containing the binary.
		const std::shared_ptr<const shader_module>& module() const {
			return module_;
		}

		/// Sets the shader module containing the binary.
		void module(std::shared_ptr<const shader_module> module) {
			module_ = std::move(module);
		}

		/// Gets the specialization data for the shader binary.
		const std::vector<char>& specialization_data() const {
			return specialization_data_;
		}

		/// Sets the specialization data for the shader binary.
		void specialization_data(size_t specialization_data_size, const void* specialization_data) {
			specialization_data_.resize(specialization_data_size);
			auto begin = reinterpret_cast<const char*>(specialization_data);
			auto end = begin + specialization_data_size;
			std::copy(begin, end, specialization_data_.begin());
			update_specialization_info();
		}

		/// Gets the specialization map for the shader binary.
		const std::vector<vk::SpecializationMapEntry>& specialization_map() const {
			return specialization_map_;
		}

		/// Adds the given entry to the specialization map for the shader binary.
		void add_specialization_map_entry(const vk::SpecializationMapEntry& specialization_map_entry) {
			specialization_map_.push_back(specialization_map_entry);
			update_specialization_info();
		}

		/// Empties the specialization map for the shader binary.
		void clear_specialization_map() {
			specialization_map_.clear();
			update_specialization_info();
		}

		/// Gets the associated shader stage.
		vk::ShaderStageFlagBits stage() const {
			return stage_;
		}

		/// Sets the associated shader stage.
		void stage(vk::ShaderStageFlagBits stage) {
			stage_ = stage;
		}

		/// Returns the corresponding create info structure for passing to vulkan.
		vk::PipelineShaderStageCreateInfo create_info();
	};

	/// Bundles the configuration data for the input stage.
	class vertex_input_state_config {
	private:
		std::vector<vk::VertexInputBindingDescription> bindings_;
		std::vector<vk::VertexInputAttributeDescription> attributes_;

	public:
		/// Creates an empty vertex_input_state.
		vertex_input_state_config() {}
		/// Creates a vertex_input_state from the given vertex input bindings and vertex input attributes.
		vertex_input_state_config(std::vector<vk::VertexInputBindingDescription> bindings,
								  std::vector<vk::VertexInputAttributeDescription> attributes)
				: bindings_{std::move(bindings)}, attributes_{std::move(attributes)} {}

		/// Gets the vertex input attributes.
		const std::vector<vk::VertexInputAttributeDescription>& attributes() const {
			return attributes_;
		}

		/// Adds the given vertex input attribute.
		void add_attribute(const vk::VertexInputAttributeDescription& attribute) {
			attributes_.push_back(attribute);
		}

		/// Empties the vertex input attributes.
		void clear_attributes() {
			attributes_.clear();
		}

		/// Gets the vertex input binding descriptions.
		const std::vector<vk::VertexInputBindingDescription>& bindings() const {
			return bindings_;
		}

		/// Adds the given vertex input binding descriptions.
		void add_binding(const vk::VertexInputBindingDescription& binding) {
			bindings_.push_back(binding);
		}

		/// Empties the vertex input bindings.
		void clear_bindings() {
			bindings_.clear();
		}

		/// Returns the corresponding create info structure for passing to vulkan.
		vk::PipelineVertexInputStateCreateInfo create_info() const;
	};

	/// Bundles the configuration data for the view port state of a pipeline.
	class viewport_state_config {
	private:
		std::vector<vk::Viewport> viewports_;
		std::vector<vk::Rect2D> scissors_;

	public:
		/// Creates an empty viewport_state_config.
		viewport_state_config() {}
		/// Creates a viewport_state_config from the given viewports and scissors.
		viewport_state_config(std::vector<vk::Viewport> viewports, std::vector<vk::Rect2D> scissors)
				: viewports_{std::move(viewports)}, scissors_{std::move(scissors)} {}

		/// Gets the scissor rectangles.
		const std::vector<vk::Rect2D>& scissors() const {
			return scissors_;
		}

		/// Adds the given scissor rectangles.
		void add_scissor(const vk::Rect2D& scissor) {
			scissors_.push_back(scissor);
		}

		/// Clears the scissor rectangles.
		void clear_scissor() {
			scissors_.clear();
		}

		/// Gets the view port dimensions.
		const std::vector<vk::Viewport>& viewports() const {
			return viewports_;
		}

		/// Adds the given view port dimensions.
		void add_viewport(const vk::Viewport& viewport) {
			viewports_.push_back(viewport);
		}

		/// Clears the view port dimensions.
		void clear_viewports() {
			viewports_.clear();
		}

		/// Returns the corresponding creation info structure to pass to vulkan.
		vk::PipelineViewportStateCreateInfo create_info() const;
	};

	/// Bundles the configuration data for the color blend state of a pipeline.
	class color_blend_state_config {
	private:
		boost::optional<vk::LogicOp> logic_op_;
		std::vector<vk::PipelineColorBlendAttachmentState> attachments_;
		std::array<float, 4> blend_constants_;

	public:
		/// Creates a color_blend_state_config from the given blending settings.
		color_blend_state_config(std::vector<vk::PipelineColorBlendAttachmentState> attachments,
								 std::array<float, 4> blend_constants = {{0.0f, 0.0f, 0.0f, 0.0f}})
				: attachments_{std::move(attachments)}, blend_constants_{blend_constants} {}

		/// Creates a color_blend_state_config from the given logic operation and blending settings.
		color_blend_state_config(vk::LogicOp logic_op,
								 std::vector<vk::PipelineColorBlendAttachmentState> attachments,
								 std::array<float, 4> blend_constants = {{0.0f, 0.0f, 0.0f, 0.0f}})
				: logic_op_{logic_op}, attachments_{std::move(attachments)}, blend_constants_{
																					 blend_constants} {}

		/// Initializes an empty color_blend_state_config.
		color_blend_state_config() {}

		/// Gets the blend states for the attachments.
		const std::vector<vk::PipelineColorBlendAttachmentState>& attachments() const {
			return attachments_;
		}

		/// Adds the given attachment blend state.
		void add_attachment(const vk::PipelineColorBlendAttachmentState& attachment) {
			attachments_.push_back(attachment);
		}

		/// Empties the attachment blend states.
		void clear_attachments() {
			attachments_.clear();
		}

		/// Allows read-only access to the blend constants array.
		const std::array<float, 4>& blend_constants() const {
			return blend_constants_;
		}

		/// Allows read-write access to the blend constants array.
		std::array<float, 4>& blend_constants() {
			return blend_constants_;
		}

		/// Provides read-only access to the logic operation.
		const boost::optional<vk::LogicOp>& logic_op() const {
			return logic_op_;
		}

		/// Provides read-write access to the logic operation.
		boost::optional<vk::LogicOp>& logic_op() {
			return logic_op_;
		}

		/// Returns the corresponding creation info structure to pass to vulkan.
		vk::PipelineColorBlendStateCreateInfo create_info() const;
	};

private:
	std::vector<shader_stage_config> shader_stages_;
	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages_ci;
	vertex_input_state_config input_state_;
	vk::PipelineVertexInputStateCreateInfo input_state_ci;
	vk::PipelineInputAssemblyStateCreateInfo assembly_state_;
	boost::optional<vk::PipelineTessellationStateCreateInfo> tesselation_state_;
	boost::optional<viewport_state_config> viewport_state_;
	boost::optional<vk::PipelineViewportStateCreateInfo> viewport_state_ci;
	vk::PipelineRasterizationStateCreateInfo rasterization_state_;
	// TODO: Handle ownership for sample mask.
	boost::optional<vk::PipelineMultisampleStateCreateInfo> multisample_state_;
	boost::optional<vk::PipelineDepthStencilStateCreateInfo> depth_stencil_state_;
	boost::optional<color_blend_state_config> color_blend_state_;
	boost::optional<vk::PipelineColorBlendStateCreateInfo> color_blend_state_ci;
	boost::optional<std::vector<vk::DynamicState>> dynamic_states_;
	boost::optional<vk::PipelineDynamicStateCreateInfo> dynamic_states_ci;
	std::shared_ptr<const pipeline_layout> layout_;
	std::shared_ptr<const render_pass> compatible_render_pass_;
	uint32_t compatible_subpass_;

public:
	/// Initializes the pipeline_config object to an empty state.
	pipeline_config();
	/// Destroys the pipeline_config object and releases associated resources.
	~pipeline_config();

	/// Provides read-only access to the vertex_input_state_config.
	const vertex_input_state_config& input_state() const {
		return input_state_;
	}

	/// Provides read-write access to the vertex_input_state_config.
	vertex_input_state_config& input_state() {
		return input_state_;
	}

	/// Provides read-only access to the shader_stage_config collection.
	const std::vector<shader_stage_config>& shader_stages() const {
		return shader_stages_;
	}

	/// Provides read-write access to the shader_stage_config collection.
	std::vector<shader_stage_config>& shader_stages() {
		return shader_stages_;
	}

	/// Provides read-only access to the input assembly state configuration.
	const vk::PipelineInputAssemblyStateCreateInfo& assembly_state() const {
		return assembly_state_;
	}

	/// Provides read-write access to the input assembly state configuration.
	vk::PipelineInputAssemblyStateCreateInfo& assembly_state() {
		return assembly_state_;
	}

	/// Provides read-only access to the tesselation state configuration.
	const boost::optional<vk::PipelineTessellationStateCreateInfo>& tesselation_state() const {
		return tesselation_state_;
	}

	/// Provides read-write access to the tesselation state configuration.
	boost::optional<vk::PipelineTessellationStateCreateInfo>& tesselation_state() {
		return tesselation_state_;
	}

	/// Provides read-only access to the viewport_state_config.
	const boost::optional<viewport_state_config>& viewport_state() const {
		return viewport_state_;
	}

	/// Provides read-write access to the viewport_state_config.
	boost::optional<viewport_state_config>& viewport_state() {
		return viewport_state_;
	}

	/// Provides read-only access to the rasterization state configuration.
	const vk::PipelineRasterizationStateCreateInfo& rasterization_state() const {
		return rasterization_state_;
	}

	/// Provides read-write access to the rasterization state configuration.
	vk::PipelineRasterizationStateCreateInfo& rasterization_state() {
		return rasterization_state_;
	}

	/// Provides read-only access to the color_blend_state_config.
	const boost::optional<color_blend_state_config>& color_blend_state() const {
		return color_blend_state_;
	}

	/// Provides read-write access to the color_blend_state_config.
	boost::optional<color_blend_state_config>& color_blend_state() {
		return color_blend_state_;
	}

	/// Provides read-only access to the depth and stencil state configuration.
	const boost::optional<vk::PipelineDepthStencilStateCreateInfo>& depth_stencil_state() const {
		return depth_stencil_state_;
	}

	/// Provides read-write access to the depth and stencil state configuration.
	boost::optional<vk::PipelineDepthStencilStateCreateInfo>& depth_stencil_state() {
		return depth_stencil_state_;
	}

	/// Provides read-only access to the dynamic states collection.
	const boost::optional<std::vector<vk::DynamicState>>& dynamic_states() const {
		return dynamic_states_;
	}

	/// Provides read-write access to the dynamic states collection.
	boost::optional<std::vector<vk::DynamicState>>& dynamic_states() {
		return dynamic_states_;
	}

	/// Gets the pipeline layout for the pipeline.
	const std::shared_ptr<const pipeline_layout>& layout() const {
		return layout_;
	}

	/// Sets the pipeline layout for the pipeline.
	void layout(const std::shared_ptr<const pipeline_layout>& layout) {
		this->layout_ = layout;
	}

	/// Provides read-only access to the multisampling state configuration.
	const boost::optional<vk::PipelineMultisampleStateCreateInfo>& multisample_state() const {
		return multisample_state_;
	}

	/// Provides read-write access to the multisampling state configuration.
	boost::optional<vk::PipelineMultisampleStateCreateInfo>& multisample_state() {
		return multisample_state_;
	}

	/// Gets the render pass with which the render pass should be compatible.
	const std::shared_ptr<const graphics::render_pass>& compatible_render_pass() const {
		return compatible_render_pass_;
	}

	/// Sets the render pass with which the render pass should be compatible.
	void compatible_render_pass(const std::shared_ptr<const graphics::render_pass>& compatible_render_pass) {
		this->compatible_render_pass_ = compatible_render_pass;
	}

	/// Gets the subpass in the render pass with which the render pass should be compatible.
	uint32_t compatible_subpass() const {
		return compatible_subpass_;
	}

	/// Sets the subpass in the render pass with which the render pass should be compatible.
	void compatible_subpass(uint32_t compatible_subpass) {
		this->compatible_subpass_ = compatible_subpass;
	}

	/// Updates pointer structures and generates a creation info structure for the whole pipeline.
	vk::GraphicsPipelineCreateInfo generate_create_info_structure();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_CONFIG_HPP_ */
