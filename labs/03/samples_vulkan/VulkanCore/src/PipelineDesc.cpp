#include <VulkanCore/Geometry.h>
#include <VulkanCore/PipelineDesc.h>

namespace vkc
{

PipelineDesc& PipelineDesc::Shaders(std::string vert, std::string frag)
{
	m_vertPath = std::move(vert);
	m_fragPath = std::move(frag);
	return *this;
}

PipelineDesc& PipelineDesc::Vertex(const Geometry& geometry)
{
	m_vertexLayout = geometry.Layout();
	return *this;
}

PipelineDesc& PipelineDesc::Topology(vk::PrimitiveTopology topology)
{
	m_topology = topology;
	return *this;
}

} // namespace vkc
