#include "ViewportApp.h"

#include <VulkanCore/Rendering.h>

ViewportApp::ViewportApp()
	: Application({ .title = "03 Область вывода" })
	, m_triangle(Device(), Triangle::Vertices())
	, m_pipeline(Device(), Shaders(),
		  vkc::PipelineDesc{}.Shaders("shader.vert", "shader.frag").Vertex(m_triangle),
		  Formats())
	, m_labInfo("03 Область вывода", "Область вывода задаёт прямоугольник кадра, в который\n"
									 "растягивается пространство отсечения.\n"
									 "Настройте её положение и размер на панели.")
{
}

void ViewportApp::OnGui()
{
	m_labInfo.Render();
	m_panel.Render(Extent());
}

void ViewportApp::OnRender(const vkc::Frame& frame)
{
	const glm::vec3 background = m_panel.ClearColor();

	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ background.r, background.g, background.b, 1.0f }) };

	pass.SetViewport(m_panel.GetViewport(frame.extent).ToRect());
	pass.Draw(m_triangle, m_pipeline);
}
