#pragma once

#include "../Model/Viewport.h"

class ViewportPanel
{
public:
	/// frame нужен, чтобы ограничить ползунки размерами кадра
	/// и задать начальные значения при первом показе.
	void Render(vk::Extent2D frame);

	/// Область вывода с учётом флажка "весь кадр".
	Viewport GetViewport(vk::Extent2D frame) const;

	glm::vec3 ClearColor() const noexcept { return m_clearColor; }

private:
	void Reset(vk::Extent2D frame);

	Viewport m_viewport{};
	glm::vec3 m_clearColor{ 0.0f, 0.0f, 0.0f };
	bool m_fullViewport = false;
	bool m_initialized = false;
};
