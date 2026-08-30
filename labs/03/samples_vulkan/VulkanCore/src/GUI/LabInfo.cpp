#include <VulkanCore/GUI/LabInfo.h>

#include <imgui.h>

#include <utility>

namespace vkc
{

LabInfo::LabInfo(std::string title, std::string description)
	: m_title(std::move(title))
	, m_description(std::move(description))
{
}

void LabInfo::Render() const
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos({ viewport->WorkPos.x + 10.0f, viewport->WorkPos.y + 10.0f },
		ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.6f);

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

	if (ImGui::Begin(m_title.c_str(), nullptr, flags))
		ImGui::TextUnformatted(m_description.c_str());

	ImGui::End();
}

} // namespace vkc
