#pragma once

#include <VulkanCore/Vertex.h>

#include <array>

struct Triangle
{
	static constexpr std::array<vkc::ColoredVertex3D, 3> Vertices()
	{
		return { {
			{ { 0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		} };
	}
};
