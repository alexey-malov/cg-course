#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace vkc
{

/// Форматы целей рендеринга. Нужны при создании пайплайна:
/// он обязан знать, во что будет писать.
struct TargetFormats
{
	vk::Format color = vk::Format::eUndefined;
};

/// Текущий кадр: чем рисовать и куда.
struct Frame
{
	vk::CommandBuffer cmd;
	uint32_t slot = 0; // номер кадра в полёте, 0..framesInFlight-1
	vk::ImageView color;
	vk::Extent2D extent;
};

/// Что делать с содержимым цели в начале прохода.
enum class LoadOp
{
	Clear,
	Load,
	DontCare
};

/// Параметры прохода рендеринга. Соответствует VkRenderingInfo.
struct RenderingInfo
{
	glm::vec4 clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	LoadOp color = LoadOp::Clear;

	static RenderingInfo Clear(glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f })
	{
		return RenderingInfo{ .clearColor = color, .color = LoadOp::Clear };
	}

	static RenderingInfo Continue() { return RenderingInfo{ .color = LoadOp::Load }; }
};

} // namespace vkc
