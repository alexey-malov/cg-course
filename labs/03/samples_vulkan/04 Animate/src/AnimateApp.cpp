#include "AnimateApp.h"

#include <VulkanCore/Rendering.h>

AnimateApp::AnimateApp()
	: Application({ .title = "04 Анимация" })
	, m_geometry(Device(), RotatingTriangle::Vertices())
	, m_pipeline(Device(), Shaders(),
		  vkc::PipelineDesc{}
			  .Shaders("shader.vert", "shader.frag")
			  .Vertex(m_geometry)
			  .Push<glm::mat4>(vk::ShaderStageFlagBits::eVertex),
		  Formats())
	, m_labInfo("04 Анимация", "Треугольник вращается с постоянной скоростью.\n"
							   "Вершины не меняются: движение задаёт матрица,\n"
							   "которая приезжает push-константой каждый кадр.")
{
	const vk::Extent2D extent = Extent();
	m_animation.SetAspect(static_cast<float>(extent.width) / static_cast<float>(extent.height));
}

void AnimateApp::OnUpdate(float dt)
{
	m_animation.Advance(dt);
}

void AnimateApp::OnResize(vk::Extent2D extent)
{
	m_animation.SetAspect(static_cast<float>(extent.width) / static_cast<float>(extent.height));
}

void AnimateApp::OnGui()
{
	m_labInfo.Render();
}

void AnimateApp::OnRender(const vkc::Frame& frame)
{
	vkc::Rendering pass{ frame, vkc::RenderingInfo::Clear({ 0.02f, 0.02f, 0.04f, 1.0f }) };
	pass.Draw(m_geometry, m_pipeline, m_animation.Transform());
}
