#include <VulkanCore/Rendering.h>

#include <algorithm>
#include <array>

namespace vkc
{

namespace
{

vk::AttachmentLoadOp ToVulkan(LoadOp op)
{
	switch (op)
	{
	case LoadOp::Clear:
		return vk::AttachmentLoadOp::eClear;
	case LoadOp::Load:
		return vk::AttachmentLoadOp::eLoad;
	default:
		return vk::AttachmentLoadOp::eDontCare;
	}
}

vk::Rect2D Intersect(vk::Rect2D a, vk::Rect2D b)
{
	const int32_t left = std::max(a.offset.x, b.offset.x);
	const int32_t top = std::max(a.offset.y, b.offset.y);

	const int32_t right = std::min(a.offset.x + static_cast<int32_t>(a.extent.width),
		b.offset.x + static_cast<int32_t>(b.extent.width));
	const int32_t bottom = std::min(a.offset.y + static_cast<int32_t>(a.extent.height),
		b.offset.y + static_cast<int32_t>(b.extent.height));

	return vk::Rect2D{ .offset = { left, top },
		.extent = { static_cast<uint32_t>(std::max(0, right - left)),
			static_cast<uint32_t>(std::max(0, bottom - top)) } };
}

} // namespace

Rendering::Rendering(const Frame& frame, const RenderingInfo& info)
	: m_cmd(frame.cmd)
	, m_frame({ .offset = { 0, 0 }, .extent = frame.extent })
	, m_viewport({ .offset = { 0, 0 }, .extent = frame.extent })
	, m_slot(frame.slot)
{
	vk::ClearValue clear{};
	clear.color = vk::ClearColorValue{ std::array<float, 4>{
		info.clearColor.r, info.clearColor.g, info.clearColor.b, info.clearColor.a } };

	const vk::RenderingAttachmentInfo colorAttachment{ .imageView = frame.color,
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = ToVulkan(info.color),
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clear };

	const vk::RenderingInfo renderingInfo{ .renderArea = m_frame,
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachment };

	m_cmd.beginRendering(renderingInfo);
}

Rendering::~Rendering()
{
	m_cmd.endRendering();
}

void Rendering::SetViewport(vk::Rect2D area)
{
	m_viewport = area;
}

void Rendering::Draw(const Geometry& geometry, const Pipeline& pipeline)
{
	DrawGeometry(geometry, pipeline, nullptr, 0);
}

void Rendering::Draw(const Pipeline& pipeline, uint32_t vertexCount)
{
	DrawProcedural(pipeline, vertexCount, nullptr, 0);
}

void Rendering::DrawGeometry(const Geometry& geometry, const Pipeline& pipeline, const void* push,
	uint32_t pushSize)
{
	pipeline.Bind(m_cmd, m_viewport, Intersect(m_viewport, m_frame));

	if (pushSize > 0)
		pipeline.Push(m_cmd, push, pushSize);

	geometry.Bind(m_cmd, m_slot);
	m_cmd.draw(geometry.Count(), 1, 0, 0);
}

void Rendering::DrawProcedural(const Pipeline& pipeline, uint32_t vertexCount, const void* push,
	uint32_t pushSize)
{
	pipeline.Bind(m_cmd, m_viewport, Intersect(m_viewport, m_frame));

	if (pushSize > 0)
		pipeline.Push(m_cmd, push, pushSize);

	m_cmd.draw(vertexCount, 1, 0, 0);
}

} // namespace vkc
