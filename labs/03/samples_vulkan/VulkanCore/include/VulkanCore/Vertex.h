#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <concepts>
#include <vector>

namespace vkc
{

/// Описание того, как байты вершинного буфера ложатся на входы вершинного шейдера.
struct VertexLayout
{
	std::vector<vk::VertexInputBindingDescription> bindings;
	std::vector<vk::VertexInputAttributeDescription> attributes;
};

/// Тип вершины — это любая структура, умеющая описать свою раскладку.
/// Свой тип вершины объявляется так же: структура со статическим Layout().
template <class T>
concept VertexType = requires {
	{ T::Layout() } -> std::same_as<VertexLayout>;
};

/// Вершина на плоскости с собственным цветом.
struct ColoredVertex2D
{
	glm::vec2 pos;
	glm::vec3 color;

	static VertexLayout Layout()
	{
		return { .bindings = { { .binding = 0,
					 .stride = sizeof(ColoredVertex2D),
					 .inputRate = vk::VertexInputRate::eVertex } },
			.attributes = {
				{ .location = 0,
					.binding = 0,
					.format = vk::Format::eR32G32Sfloat,
					.offset = offsetof(ColoredVertex2D, pos) },
				{ .location = 1,
					.binding = 0,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = offsetof(ColoredVertex2D, color) },
			} };
	}
};

/// Вершина в пространстве с собственным цветом. Пример 01.
struct ColoredVertex3D
{
	glm::vec3 pos;
	glm::vec3 color;

	static VertexLayout Layout()
	{
		return { .bindings = { { .binding = 0,
					 .stride = sizeof(ColoredVertex3D),
					 .inputRate = vk::VertexInputRate::eVertex } },
			.attributes = {
				{ .location = 0,
					.binding = 0,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = offsetof(ColoredVertex3D, pos) },
				{ .location = 1,
					.binding = 0,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = offsetof(ColoredVertex3D, color) },
			} };
	}
};

} // namespace vkc
