#pragma once

#include "Orbit.h"

#include <VulkanCore/Vertex.h>

#include <string>

struct PointVertex
{
	glm::vec2 pos;
	glm::vec3 color;
	float size;

	static vkc::VertexLayout Layout()
	{
		return { .bindings = { { .binding = 0,
					 .stride = sizeof(PointVertex),
					 .inputRate = vk::VertexInputRate::eVertex } },
			.attributes = {
				{ .location = 0,
					.binding = 0,
					.format = vk::Format::eR32G32Sfloat,
					.offset = offsetof(PointVertex, pos) },
				{ .location = 1,
					.binding = 0,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = offsetof(PointVertex, color) },
				{ .location = 2,
					.binding = 0,
					.format = vk::Format::eR32Sfloat,
					.offset = offsetof(PointVertex, size) },
			} };
	}
};

/// Планета: орбита плюс то, как её рисовать.
class Planet
{
public:
	Planet(std::string name, Orbit orbit, glm::vec3 color, float size);

	const std::string& Name() const noexcept { return m_name; }
	const Orbit& GetOrbit() const noexcept { return m_orbit; }
	PointVertex PointAt(double time, float zoom) const;

private:
	std::string m_name;
	Orbit m_orbit;
	glm::vec3 m_color;
	float m_size;
};
