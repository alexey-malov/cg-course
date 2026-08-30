#pragma once

#include <VulkanCore/Vertex.h>

#include <string>
#include <type_traits>

namespace vkc
{

class Geometry;

class PipelineDesc
{
public:
	PipelineDesc& Shaders(std::string vert, std::string frag);

	PipelineDesc& Vertex(const Geometry& geometry);

	template <class T>
	PipelineDesc& Push(vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex)
	{
		static_assert(std::is_trivially_copyable_v<T>, "push-константы копируются побайтово");
		m_pushSize = static_cast<uint32_t>(sizeof(T));
		m_pushStages = stages;
		return *this;
	}

	PipelineDesc& Topology(vk::PrimitiveTopology topology);

private:
	friend class Pipeline;

	std::string m_vertPath;
	std::string m_fragPath;
	VertexLayout m_vertexLayout;

	uint32_t m_pushSize = 0;
	vk::ShaderStageFlags m_pushStages = vk::ShaderStageFlagBits::eVertex;

	vk::PrimitiveTopology m_topology = vk::PrimitiveTopology::eTriangleList;
};

} // namespace vkc
