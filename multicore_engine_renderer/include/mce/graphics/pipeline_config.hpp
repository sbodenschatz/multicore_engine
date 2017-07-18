/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/pipeline_config.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_CONFIG_HPP_
#define GRAPHICS_PIPELINE_CONFIG_HPP_

#include <algorithm>
#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

class pipeline_config {
public:
	/// Bundles all data for a shader stage.
	class shader_stage_config {
	private:
		vk::ShaderStageFlagBits stage_;
		vk::ShaderModule module_;
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
		/// Gets the name of the entry point.
		const std::string& entry_point_name() const {
			return entry_point_name_;
		}

		/// Sets the name of the entry point.
		void entry_point_name(const std::string& entry_point_name) {
			entry_point_name_ = entry_point_name;
		}

		/// Gets the shader module containing the binary.
		vk::ShaderModule module() const {
			return module_;
		}

		/// Sets the shader module containing the binary.
		void module(vk::ShaderModule module) {
			module_ = module;
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
		vk::PipelineShaderStageCreateInfo create_info() const;
	};

	/// Bundles the configuration data for the input stage.
	class vertex_input_state_config {
	private:
		std::vector<vk::VertexInputBindingDescription> bindings_;
		std::vector<vk::VertexInputAttributeDescription> attributes_;

	public:
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

	class viewport_state_config {
	private:
		std::vector<vk::Viewport> viewports_;
		std::vector<vk::Rect2D> scissors_;

	public:
		const std::vector<vk::Rect2D>& scissors() const {
			return scissors_;
		}

		void add_scissor(const vk::Rect2D& scissor) {
			scissors_.push_back(scissor);
		}

		void clear_scissor() {
			scissors_.clear();
		}

		const std::vector<vk::Viewport>& viewports() const {
			return viewports_;
		}

		void add_viewport(const vk::Viewport& viewport) {
			viewports_.push_back(viewport);
		}

		void clear_viewports() {
			viewports_.clear();
		}

		vk::PipelineViewportStateCreateInfo create_info() const;
	};

	class color_blend_state_config {
	private:
		bool logicOpEnable;
		vk::LogicOp logicOp;
		std::vector<vk::PipelineColorBlendAttachmentState> attachments_;
		std::array<float, 4> blendConstants;

	public:
		color_blend_state_config() : logicOpEnable{false}, logicOp{vk::LogicOp::eSet} {}
		const std::vector<vk::PipelineColorBlendAttachmentState>& attachments() const {
			return attachments_;
		}

		void add_attachment(const vk::PipelineColorBlendAttachmentState& attachment) {
			attachments_.push_back(attachment);
		}

		void clear_attachments() {
			attachments_.clear();
		}

		const std::array<float, 4>& blend_constants() const {
			return blendConstants;
		}

		std::array<float, 4>& blend_constants() {
			return blendConstants;
		}

		vk::LogicOp logic_op() const {
			return logicOp;
		}

		void logic_op(vk::LogicOp logic_op) {
			logicOp = logic_op;
		}

		bool logic_op_enable() const {
			return logicOpEnable;
		}

		void logic_op_enable(bool logic_op_enable) {
			logicOpEnable = logic_op_enable;
		}

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
	boost::optional<vk::PipelineMultisampleStateCreateInfo> multisample_state_;
	boost::optional<vk::PipelineDepthStencilStateCreateInfo> depth_stencil_state_;
	boost::optional<color_blend_state_config> color_blend_state_;
	boost::optional<vk::PipelineColorBlendStateCreateInfo> color_blend_state_ci;
	boost::optional<std::vector<vk::DynamicState>> dynamic_states_;
	boost::optional<vk::PipelineDynamicStateCreateInfo> dynamic_states_ci;
	vk::PipelineLayout layout_;
	vk::RenderPass render_pass_;
	uint32_t subpass_;

public:
	pipeline_config();
	~pipeline_config();

	const vertex_input_state_config& input_state() const {
		return input_state_;
	}

	vertex_input_state_config& input_state() {
		return input_state_;
	}

	const std::vector<shader_stage_config>& shader_stages() const {
		return shader_stages_;
	}

	std::vector<shader_stage_config>& shader_stages() {
		return shader_stages_;
	}

	const vk::PipelineInputAssemblyStateCreateInfo& assembly_state() const {
		return assembly_state_;
	}

	vk::PipelineInputAssemblyStateCreateInfo& assembly_state() {
		return assembly_state_;
	}

	const boost::optional<vk::PipelineTessellationStateCreateInfo>& tesselation_state() const {
		return tesselation_state_;
	}

	boost::optional<vk::PipelineTessellationStateCreateInfo>& tesselation_state() {
		return tesselation_state_;
	}

	const boost::optional<viewport_state_config>& viewport_state() const {
		return viewport_state_;
	}

	boost::optional<viewport_state_config>& viewport_state() {
		return viewport_state_;
	}

	const vk::PipelineRasterizationStateCreateInfo& rasterization_state() const {
		return rasterization_state_;
	}

	vk::PipelineRasterizationStateCreateInfo& rasterization_state() {
		return rasterization_state_;
	}

	const boost::optional<color_blend_state_config>& color_blend_state() const {
		return color_blend_state_;
	}

	boost::optional<color_blend_state_config>& color_blend_state() {
		return color_blend_state_;
	}

	const boost::optional<vk::PipelineDepthStencilStateCreateInfo>& depth_stencil_state() const {
		return depth_stencil_state_;
	}

	boost::optional<vk::PipelineDepthStencilStateCreateInfo>& depth_stencil_state() {
		return depth_stencil_state_;
	}

	const boost::optional<std::vector<vk::DynamicState>>& dynamic_states() const {
		return dynamic_states_;
	}

	boost::optional<std::vector<vk::DynamicState>>& dynamic_states() {
		return dynamic_states_;
	}

	vk::PipelineLayout layout() const {
		return layout_;
	}

	void layout(vk::PipelineLayout layout) {
		this->layout_ = layout;
	}

	const boost::optional<vk::PipelineMultisampleStateCreateInfo>& multisample_state() const {
		return multisample_state_;
	}
	boost::optional<vk::PipelineMultisampleStateCreateInfo>& multisample_state() {
		return multisample_state_;
	}

	vk::RenderPass render_pass() const {
		return render_pass_;
	}

	void render_pass(vk::RenderPass render_pass) {
		this->render_pass_ = render_pass;
	}

	uint32_t subpass() const {
		return subpass_;
	}

	void subpass(uint32_t subpass) {
		this->subpass_ = subpass;
	}
	vk::GraphicsPipelineCreateInfo generate_create_info_structure();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_CONFIG_HPP_ */
