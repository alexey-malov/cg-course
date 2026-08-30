#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

/// Прямоугольник, на который растягивается пространство отсечения.
///
/// Значимый тип: копируется, не владеет ничем. Вся суть примера в том, что
/// треугольник фиксированного размера занимает прямоугольник любой формы
/// и в любом месте кадра — форму и место задаёт именно этот тип.
///
/// Границами кадра он не ограничен: если сдвинуть его вправо, картинка
/// частично уедет за экран, но не сплющится. Обрезка по кадру — забота
/// прямоугольника отсечения, и она форму изображения не меняет.
struct Viewport
{
    glm::ivec2 offset{ 0, 0 };
    glm::ivec2 size{ 0, 0 };

    static Viewport Full(vk::Extent2D frame)
    {
        return Viewport{ .offset = { 0, 0 },
                         .size   = { static_cast<int>(frame.width),
                                     static_cast<int>(frame.height) } };
    }

    vk::Rect2D ToRect() const
    {
        return vk::Rect2D{ .offset = { offset.x, offset.y },
                           .extent = { static_cast<uint32_t>(size.x),
                                       static_cast<uint32_t>(size.y) } };
    }

    float Aspect() const
    {
        return size.y > 0 ? static_cast<float>(size.x) / static_cast<float>(size.y) : 1.0f;
    }
};
