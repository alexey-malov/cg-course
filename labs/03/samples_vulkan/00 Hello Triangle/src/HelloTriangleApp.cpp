#include "HelloTriangleApp.h"

#include <VulkanCore/Rendering.h>

HelloTriangleApp::HelloTriangleApp()
	: Application({ .title = "00 Треугольник", .gui = false })
	, m_pipeline(Device(), Shaders(), vkc::PipelineDesc{}.Shaders("shader.vert", "shader.frag"),
		  Formats())
{
}

void HelloTriangleApp::OnRender(const vkc::Frame& frame)
{
	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ 0.02f, 0.02f, 0.04f, 1.0f }) };
	pass.Draw(m_pipeline, 3);
}
