#pragma once

#include "Model/DraggableTriangle.h"

#include <VulkanCore/Application.h>
#include <VulkanCore/GUI/LabInfo.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

class DragApp : public vkc::Application
{
public:
	DragApp();

protected:
	void OnPrepare(const vkc::Frame& frame) override;
	void OnGui() override;
	void OnRender(const vkc::Frame& frame) override;

	void OnMouseButton(glm::vec2 pos, int button, int action, int mods) override;
	void OnMouseMove(glm::vec2 pos, glm::vec2 delta) override;

private:
	/// Из координат окна в пространство отсечения.
	glm::vec2 ToClipSpace(glm::vec2 screen) const;

	DraggableTriangle m_triangle;
	vkc::Geometry m_geometry;
	vkc::Pipeline m_pipeline;
	vkc::LabInfo m_labInfo;
};
