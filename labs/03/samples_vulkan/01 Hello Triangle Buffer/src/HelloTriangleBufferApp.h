#pragma once

#include "Model/Triangle.h"

#include <VulkanCore/Application.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

class HelloTriangleBufferApp : public vkc::Application
{
public:
	HelloTriangleBufferApp();

protected:
	void OnRender(const vkc::Frame& frame) override;

private:
	vkc::Geometry m_triangle;
	vkc::Pipeline m_pipeline;
};
