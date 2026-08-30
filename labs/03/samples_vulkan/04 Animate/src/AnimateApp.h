#pragma once

#include "Model/RotatingTriangle.h"

#include <VulkanCore/Application.h>
#include <VulkanCore/GUI/LabInfo.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

class AnimateApp : public vkc::Application
{
public:
	AnimateApp();

protected:
	void OnUpdate(float dt) override;
	void OnGui() override;
	void OnRender(const vkc::Frame& frame) override;
	void OnResize(vk::Extent2D extent) override;

private:
	RotatingTriangle m_animation;
	vkc::Geometry m_geometry;
	vkc::Pipeline m_pipeline;
	vkc::LabInfo m_labInfo;
};
