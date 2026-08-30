#include "OrbitApp.h"

#include <VulkanCore/Rendering.h>

#include <cmath>

OrbitApp::OrbitApp()
	: Application({ .title = "05 Расчёт орбит" })
	, m_orbits(Device(), m_system.OrbitPoints())
	, m_bodies(Device(), m_system.BodyPoints(), FramesInFlight())
	, m_pipeline(Device(), Shaders(),
		  vkc::PipelineDesc{}
			  .Shaders("shader.vert", "shader.frag")
			  .Vertex(m_orbits)
			  .Push<ViewScale>(vk::ShaderStageFlagBits::eVertex)
			  .Topology(vk::PrimitiveTopology::ePointList),
		  Formats())
	, m_labInfo("05 Расчёт орбит", "Солнечная система по законам Кеплера.\n"
								   "Колесо мыши: масштаб. Ползунок: скорость времени.")
{
}

OrbitApp::ViewScale OrbitApp::CurrentScale() const
{
	const vk::Extent2D extent = Extent();
	const float zoom = m_system.Zoom();

	return ViewScale{ zoom * 2.0f / static_cast<float>(extent.width),
		zoom * 2.0f / static_cast<float>(extent.height) };
}

void OrbitApp::OnUpdate(float dt)
{
	m_system.Advance(dt);
}

void OrbitApp::OnPrepare(const vkc::Frame& frame)
{
	m_bodies.Update(frame, m_system.BodyPoints());
}

void OrbitApp::OnGui()
{
	m_labInfo.Render();
	m_controlPanel.Render(m_system);
}

void OrbitApp::OnScroll(glm::vec2 offset)
{
	if (offset.y != 0.0f)
		m_system.ZoomBy(std::pow(1.1f, offset.y));
}

void OrbitApp::OnRender(const vkc::Frame& frame)
{
	const ViewScale scale = CurrentScale();

	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ 0.01f, 0.01f, 0.03f, 1.0f }) };
	pass.Draw(m_orbits, m_pipeline, scale);
	pass.Draw(m_bodies, m_pipeline, scale);
}
