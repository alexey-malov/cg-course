#include "ControlPanel.h"

#include "../Model/SolarSystem.h"

#include <imgui.h>

void ControlPanel::Render(SolarSystem& system)
{
    ImGui::SetNextWindowBgAlpha(0.6f);

    if (ImGui::Begin("Симуляция", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Год: %d", system.Year());
        ImGui::Separator();

        float speed = system.TimeSpeed();
        if (ImGui::SliderFloat("Лет в секунду", &speed, -20.0f, 20.0f, "%.2f"))
            system.SetTimeSpeed(speed);

        ImGui::Text("Масштаб: %.1f", static_cast<double>(system.Zoom()));
        ImGui::TextUnformatted("Колесо мыши: приблизить или отдалить");

        ImGui::Separator();
        ImGui::TextWrapped("Тела движутся по эллипсам неравномерно: у перицентра быстрее, "
                           "у апоцентра медленнее. Заметнее всего на комете Галлея.");
    }
    ImGui::End();
}
