/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/pipeline_config.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <iterator>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/graphics/shader_module.hpp>

namespace mce {
namespace graphics {

vk::PipelineShaderStageCreateInfo pipeline_config::shader_stage_config::create_info() {
	update_specialization_info();
	vk::PipelineShaderStageCreateInfo ci;
	ci.stage = stage_;
	ci.pName = entry_point_name_.c_str();
	ci.module = module_->native_shader_module();
	if(specialization_data_.size() || specialization_map_.size()) {
		ci.pSpecializationInfo = &specialization_info_;
	}
	return ci;
}
vk::PipelineVertexInputStateCreateInfo pipeline_config::vertex_input_state_config::create_info() const {
	vk::PipelineVertexInputStateCreateInfo ci;
	ci.vertexBindingDescriptionCount = uint32_t(bindings_.size());
	ci.pVertexBindingDescriptions = bindings_.data();
	ci.vertexAttributeDescriptionCount = uint32_t(attributes_.size());
	ci.pVertexAttributeDescriptions = attributes_.data();
	return ci;
}

vk::PipelineViewportStateCreateInfo pipeline_config::viewport_state_config::create_info() const {
	vk::PipelineViewportStateCreateInfo ci;
	ci.scissorCount = uint32_t(scissors_.size());
	ci.pScissors = scissors_.data();
	ci.viewportCount = uint32_t(viewports_.size());
	ci.pViewports = viewports_.data();
	return ci;
}

vk::PipelineColorBlendStateCreateInfo pipeline_config::color_blend_state_config::create_info() const {
	vk::PipelineColorBlendStateCreateInfo ci;
	ci.logicOpEnable = logicOpEnable;
	ci.logicOp = logicOp;
	ci.attachmentCount = uint32_t(attachments_.size());
	ci.pAttachments = attachments_.data();
	ci.setBlendConstants(blendConstants);
	return ci;
}

vk::GraphicsPipelineCreateInfo pipeline_config::generate_create_info_structure() {
	vk::GraphicsPipelineCreateInfo ci;
	shader_stages_ci.clear();
	std::transform(shader_stages_.begin(), shader_stages_.end(), std::back_inserter(shader_stages_ci),
				   [](shader_stage_config& ssc) { return ssc.create_info(); });
	ci.stageCount = uint32_t(shader_stages_ci.size());
	ci.pStages = shader_stages_ci.data();
	input_state_ci = input_state_.create_info();
	ci.pVertexInputState = &input_state_ci;
	ci.pInputAssemblyState = &assembly_state_;
	if(tesselation_state_) {
		ci.pTessellationState = &tesselation_state_.get();
	}
	if(viewport_state_) {
		viewport_state_ci = viewport_state_->create_info();
		ci.pViewportState = &viewport_state_ci.get();
	} else {
		viewport_state_ci.reset();
	}
	ci.pRasterizationState = &rasterization_state_;
	if(multisample_state_) {
		ci.pMultisampleState = &multisample_state_.get();
	}
	if(depth_stencil_state_) {
		ci.pDepthStencilState = &depth_stencil_state_.get();
	}
	if(color_blend_state_) {
		color_blend_state_ci = color_blend_state_->create_info();
		ci.pColorBlendState = &color_blend_state_ci.get();
	} else {
		color_blend_state_ci.reset();
	}
	if(dynamic_states_) {
		dynamic_states_ci = vk::PipelineDynamicStateCreateInfo(vk::PipelineDynamicStateCreateFlags(),
															   uint32_t(dynamic_states_->size()),
															   dynamic_states_->data());
		ci.pDynamicState = &dynamic_states_ci.get();
	} else {
		dynamic_states_ci.reset();
	}
	ci.layout = layout_;
	ci.renderPass = render_pass_;
	ci.subpass = subpass_;
	return ci;
}

pipeline_config::pipeline_config() : subpass_(0) {}

pipeline_config::~pipeline_config() {}

} /* namespace graphics */
} /* namespace mce */
