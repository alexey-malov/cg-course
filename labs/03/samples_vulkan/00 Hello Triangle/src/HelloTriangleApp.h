#pragma once

#include <VulkanCore/Application.h>
#include <VulkanCore/Pipeline.h>

class HelloTriangleApp : public vkc::Application
{
public:
	HelloTriangleApp();

protected:
	void OnRender(const vkc::Frame& frame) override;

private:
	vkc::Pipeline m_pipeline;
};
