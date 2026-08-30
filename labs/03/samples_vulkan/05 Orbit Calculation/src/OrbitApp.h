#pragma once

#include "GUI/ControlPanel.h"
#include "Model/SolarSystem.h"

#include <VulkanCore/Application.h>
#include <VulkanCore/GUI/LabInfo.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

class OrbitApp : public vkc::Application
{
public:
	OrbitApp();

protected:
	void OnUpdate(float dt) override;
	void OnPrepare(const vkc::Frame& frame) override;
	void OnGui() override;
	void OnRender(const vkc::Frame& frame) override;
	void OnScroll(glm::vec2 offset) override;

private:
	struct ViewScale
	{
		float x;
		float y;
	};

	ViewScale CurrentScale() const;

	SolarSystem m_system;

	vkc::Geometry m_orbits;
	vkc::Geometry m_bodies;
	vkc::Pipeline m_pipeline;

	vkc::LabInfo m_labInfo;
	ControlPanel m_controlPanel;
};
