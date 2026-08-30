#include "DragApp.h"

#include <VulkanCore/Rendering.h>

#include <GLFW/glfw3.h>

DragApp::DragApp()
	: Application({ .title = "02 Перетаскивание вершин" })
	, m_geometry(Device(), m_triangle.Vertices(), FramesInFlight())
	, m_pipeline(Device(), Shaders(),
		  vkc::PipelineDesc{}.Shaders("shader.vert", "shader.frag").Vertex(m_geometry),
		  Formats())
	, m_labInfo("02 Перетаскивание вершин", "Потяните любую вершину левой кнопкой мыши.\n"
											"Захваченная вершина подсвечивается белым.")
{
}

glm::vec2 DragApp::ToClipSpace(glm::vec2 screen) const
{
	const vk::Extent2D extent = Extent();
	if (extent.width == 0 || extent.height == 0)
		return glm::vec2{ 0.0f };

	return { screen.x / static_cast<float>(extent.width) * 2.0f - 1.0f,
		screen.y / static_cast<float>(extent.height) * 2.0f - 1.0f };
}

void DragApp::OnMouseButton(glm::vec2 pos, int button, int action, int /*mods*/)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	if (action == GLFW_PRESS)
	{
		if (const auto vertex = m_triangle.Find(ToClipSpace(pos)))
			m_triangle.Grab(*vertex);
	}
	else if (action == GLFW_RELEASE)
	{
		m_triangle.Release();
	}
}

void DragApp::OnMouseMove(glm::vec2 pos, glm::vec2 /*delta*/)
{
	m_triangle.MoveTo(ToClipSpace(pos));
}

void DragApp::OnPrepare(const vkc::Frame& frame)
{
	m_geometry.Update(frame, m_triangle.Vertices());
}

void DragApp::OnGui()
{
	m_labInfo.Render();
}

void DragApp::OnRender(const vkc::Frame& frame)
{
	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ 0.02f, 0.02f, 0.04f, 1.0f }) };
	pass.Draw(m_geometry, m_pipeline);
}
