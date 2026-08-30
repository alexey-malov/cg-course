#include "HelloTriangleBufferApp.h"

#include <VulkanCore/Rendering.h>

HelloTriangleBufferApp::HelloTriangleBufferApp()
	: Application({ .title = "01 Треугольник из буфера", .gui = false })
	, m_triangle(Device(), Triangle::Vertices())
	, m_pipeline(Device(), Shaders(),
		  vkc::PipelineDesc{}.Shaders("shader.vert", "shader.frag").Vertex(m_triangle),
		  Formats())
{
}

void HelloTriangleBufferApp::OnRender(const vkc::Frame& frame)
{
	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ 0.02f, 0.02f, 0.04f, 1.0f }) };
	pass.Draw(m_triangle, m_pipeline);
}
