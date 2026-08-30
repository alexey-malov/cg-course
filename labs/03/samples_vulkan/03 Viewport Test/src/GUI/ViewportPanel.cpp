#include "ViewportPanel.h"

#include <imgui.h>

void ViewportPanel::Reset(vk::Extent2D frame)
{
	m_viewport.offset = { 0, 0 };
	m_viewport.size = { static_cast<int>(frame.width) / 2, static_cast<int>(frame.height) / 2 };
	m_fullViewport = false;
	m_clearColor = glm::vec3{ 0.0f };
}

Viewport ViewportPanel::GetViewport(vk::Extent2D frame) const
{
	return m_fullViewport ? Viewport::Full(frame) : m_viewport;
}

void ViewportPanel::Render(vk::Extent2D frame)
{
	if (!m_initialized)
	{
		Reset(frame);
		m_initialized = true;
	}

	const int frameWidth = static_cast<int>(frame.width);
	const int frameHeight = static_cast<int>(frame.height);

	ImGui::SetNextWindowBgAlpha(0.6f);
	if (ImGui::Begin("Область вывода", nullptr,
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::BeginDisabled(m_fullViewport);
		ImGui::SliderInt("Смещение X", &m_viewport.offset.x, 0, frameWidth);
		ImGui::SliderInt("Смещение Y", &m_viewport.offset.y, 0, frameHeight);
		ImGui::SliderInt("Ширина", &m_viewport.size.x, 1, frameWidth);
		ImGui::SliderInt("Высота", &m_viewport.size.y, 1, frameHeight);
		ImGui::EndDisabled();

		ImGui::Checkbox("Весь кадр", &m_fullViewport);
		ImGui::ColorEdit3("Цвет фона", &m_clearColor.x);

		if (ImGui::Button("Сбросить"))
			Reset(frame);
	}
	ImGui::End();
}
