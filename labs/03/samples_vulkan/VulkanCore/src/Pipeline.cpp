#include <VulkanCore/Pipeline.h>
#include <VulkanCore/ShaderCache.h>

#include <array>
#include <stdexcept>

namespace vkc
{

Pipeline::Pipeline(const RenderDevice& device, ShaderCache& shaders, const PipelineDesc& desc,
	const TargetFormats& formats)
	: m_pushSize(desc.m_pushSize)
	, m_pushStages(desc.m_pushStages)
{
	// layout
	const vk::PushConstantRange pushRange{ .stageFlags = desc.m_pushStages,
		.offset = 0,
		.size = desc.m_pushSize };

	const vk::PipelineLayoutCreateInfo layoutInfo{ .pushConstantRangeCount = desc.m_pushSize > 0 ? 1u : 0u,
		.pPushConstantRanges = desc.m_pushSize > 0 ? &pushRange : nullptr };

	m_layout = vk::raii::PipelineLayout{ device.Raii(), layoutInfo };

	// стадии
	const std::array stages{
		vk::PipelineShaderStageCreateInfo{ .stage = vk::ShaderStageFlagBits::eVertex,
			.module = shaders.Get(desc.m_vertPath),
			.pName = "main" },
		vk::PipelineShaderStageCreateInfo{ .stage = vk::ShaderStageFlagBits::eFragment,
			.module = shaders.Get(desc.m_fragPath),
			.pName = "main" },
	};

	const vk::PipelineVertexInputStateCreateInfo vertexInput{
		.vertexBindingDescriptionCount = static_cast<uint32_t>(desc.m_vertexLayout.bindings.size()),
		.pVertexBindingDescriptions = desc.m_vertexLayout.bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(desc.m_vertexLayout.attributes.size()),
		.pVertexAttributeDescriptions = desc.m_vertexLayout.attributes.data()
	};

	const vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = desc.m_topology };

	// Значения задаются командой на каждый кадр, поэтому пайплайн не зависит
	// от размера окна и не требует пересоздания при его изменении.
	const vk::PipelineViewportStateCreateInfo viewport{ .viewportCount = 1, .scissorCount = 1 };

	const vk::PipelineRasterizationStateCreateInfo rasterization{
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eNone,
		.frontFace = vk::FrontFace::eCounterClockwise,
		.lineWidth = 1.0f
	};

	const vk::PipelineMultisampleStateCreateInfo multisample{ .rasterizationSamples = vk::SampleCountFlagBits::e1 };

	// Смешивание выключено: цвет фрагмента затирает то, что уже лежит
	// в изображении, а не подмешивается к нему.
	const vk::PipelineColorBlendAttachmentState blendAttachment{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
			| vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};

	const vk::PipelineColorBlendStateCreateInfo colorBlend{ .attachmentCount = 1,
		.pAttachments = &blendAttachment };

	constexpr std::array dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	const vk::PipelineDynamicStateCreateInfo dynamic{ .dynamicStateCount = static_cast<uint32_t>(
														  dynamicStates.size()),
		.pDynamicStates = dynamicStates.data() };

	// Динамический рендеринг: форматы целей объявляются здесь,
	// объекта прохода рендеринга не существует.
	const vk::PipelineRenderingCreateInfo renderingInfo{ .colorAttachmentCount = 1,
		.pColorAttachmentFormats = &formats.color };

	const vk::GraphicsPipelineCreateInfo pipelineInfo{ .pNext = &renderingInfo,
		.stageCount = static_cast<uint32_t>(stages.size()),
		.pStages = stages.data(),
		.pVertexInputState = &vertexInput,
		.pInputAssemblyState = &inputAssembly,
		.pViewportState = &viewport,
		.pRasterizationState = &rasterization,
		.pMultisampleState = &multisample,
		.pColorBlendState = &colorBlend,
		.pDynamicState = &dynamic,
		.layout = *m_layout };

	m_pipeline = vk::raii::Pipeline{ device.Raii(), nullptr, pipelineInfo };
}

void Pipeline::Bind(vk::CommandBuffer cmd, vk::Rect2D area, vk::Rect2D scissor) const
{
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);

	const vk::Viewport viewport{ .x = static_cast<float>(area.offset.x),
		.y = static_cast<float>(area.offset.y),
		.width = static_cast<float>(area.extent.width),
		.height = static_cast<float>(area.extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f };
	cmd.setViewport(0, viewport);
	cmd.setScissor(0, scissor);
}

void Pipeline::Push(vk::CommandBuffer cmd, const void* data, uint32_t size) const
{
	if (m_pushSize == 0)
		throw std::runtime_error("пайплайн создан без push-констант, а рисование их передаёт");

	if (size != m_pushSize)
		throw std::runtime_error("размер push-констант не совпадает с объявленным в PipelineDesc");

	cmd.pushConstants(*m_layout, m_pushStages, 0,
		vk::ArrayProxy<const std::byte>{ size, static_cast<const std::byte*>(data) });
}

} // namespace vkc
