#include "DraggableTriangle.h"

#include <glm/geometric.hpp>

DraggableTriangle::DraggableTriangle()
{
	m_vertices = { {
		{ { -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { 0.0f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
	} };

	for (std::size_t i = 0; i < VertexCount; ++i)
		m_baseColors[i] = m_vertices[i].color;
}

std::optional<std::size_t> DraggableTriangle::Find(glm::vec2 point, float radius) const
{
	std::optional<std::size_t> nearest;
	float nearestDistance = radius;

	for (std::size_t i = 0; i < VertexCount; ++i)
	{
		const float distance = glm::distance(point, m_vertices[i].pos);
		if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearest = i;
		}
	}

	return nearest;
}

void DraggableTriangle::Grab(std::size_t vertex)
{
	m_grabbed = vertex;
	m_vertices[vertex].color = glm::vec3{ 1.0f };
}

void DraggableTriangle::MoveTo(glm::vec2 point)
{
	if (!m_grabbed.has_value())
		return;

	m_vertices[*m_grabbed].pos = point;
}

void DraggableTriangle::Release()
{
	if (!m_grabbed.has_value())
		return;

	m_vertices[*m_grabbed].color = m_baseColors[*m_grabbed];
	m_grabbed.reset();
}
