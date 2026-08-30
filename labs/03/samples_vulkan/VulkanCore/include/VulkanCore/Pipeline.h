#pragma once

#include <VulkanCore/Frame.h>
#include <VulkanCore/PipelineDesc.h>
#include <VulkanCore/RenderDevice.h>

namespace vkc
{

class ShaderCache;

/// Собранный графический пайплайн вместе со своим layout.
class Pipeline
{
public:
	Pipeline(const RenderDevice& device, ShaderCache& shaders, const PipelineDesc& desc,
		const TargetFormats& formats);

	Pipeline(Pipeline&&) noexcept = default;
	Pipeline& operator=(Pipeline&&) noexcept = default;

	vk::PipelineLayout Layout() const noexcept { return *m_layout; }

private:
	friend class Rendering;

	/// area может выходить за пределы кадра, scissor обязан лежать внутри него.
	void Bind(vk::CommandBuffer cmd, vk::Rect2D area, vk::Rect2D scissor) const;
	void Push(vk::CommandBuffer cmd, const void* data, uint32_t size) const;

	vk::raii::PipelineLayout m_layout{ nullptr };
	vk::raii::Pipeline m_pipeline{ nullptr };

	uint32_t m_pushSize = 0;
	vk::ShaderStageFlags m_pushStages = vk::ShaderStageFlagBits::eVertex;
};

} // namespace vkc
