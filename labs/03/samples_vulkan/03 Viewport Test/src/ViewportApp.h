#pragma once

#include "GUI/ViewportPanel.h"
#include "Model/Triangle.h"

#include <VulkanCore/Application.h>
#include <VulkanCore/GUI/LabInfo.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

class ViewportApp : public vkc::Application
{
public:
	ViewportApp();

protected:
	void OnGui() override;
	void OnRender(const vkc::Frame& frame) override;

private:
	vkc::Geometry m_triangle;
	vkc::Pipeline m_pipeline;
	vkc::LabInfo m_labInfo;
	ViewportPanel m_panel;
};
