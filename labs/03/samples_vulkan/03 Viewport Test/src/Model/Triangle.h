#pragma once

#include <VulkanCore/Vertex.h>

#include <array>

/// Треугольник, заведомо больший, чем пространство отсечения.
///
/// Координаты выходят за пределы отрезка от -1 до 1 намеренно: так видно,
/// что фигура обрезается по краям области вывода, а не по краям окна.
struct Triangle
{
    static constexpr std::array<vkc::ColoredVertex2D, 3> Vertices()
    {
        return { {
            { { -1.2f, -1.2f }, { 1.0f, 0.0f, 0.0f } },
            { { 1.2f, -1.2f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 1.2f }, { 0.0f, 0.0f, 1.0f } },
        } };
    }
};
