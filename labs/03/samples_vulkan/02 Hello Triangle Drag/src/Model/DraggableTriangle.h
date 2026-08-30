#pragma once

#include <VulkanCore/Vertex.h>

#include <array>
#include <optional>

class DraggableTriangle
{
public:
	static constexpr std::size_t VertexCount = 3;

	DraggableTriangle();

	/// Ближайшая к точке вершина, если она не дальше radius. Иначе — пусто.
	std::optional<std::size_t> Find(glm::vec2 point, float radius = 0.1f) const;

	/// Берёт вершину: пока она захвачена, её цвет меняется на белый.
	void Grab(std::size_t vertex);

	/// Переносит захваченную вершину. Без захвата не делает ничего.
	void MoveTo(glm::vec2 point);

	/// Отпускает вершину и возвращает ей исходный цвет.
	void Release();

	bool IsDragging() const noexcept { return m_grabbed.has_value(); }

	const std::array<vkc::ColoredVertex2D, VertexCount>& Vertices() const noexcept
	{
		return m_vertices;
	}

private:
	std::array<vkc::ColoredVertex2D, VertexCount> m_vertices{};
	std::array<glm::vec3, VertexCount> m_baseColors{};
	std::optional<std::size_t> m_grabbed;
};
