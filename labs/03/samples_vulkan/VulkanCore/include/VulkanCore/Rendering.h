#pragma once

#include <VulkanCore/Frame.h>
#include <VulkanCore/Geometry.h>
#include <VulkanCore/Pipeline.h>

#include <type_traits>

namespace vkc
{

class Rendering
{
public:
	explicit Rendering(const Frame& frame, const RenderingInfo& info = RenderingInfo::Clear());
	~Rendering();

	Rendering(const Rendering&) = delete;
	Rendering& operator=(const Rendering&) = delete;

	/// Прямоугольник, на который растягивается пространство отсечения.
	/// По умолчанию — весь кадр, действует до конца прохода. Может выходить
	/// за границы кадра: тогда картинка частично уезжает за экран.
	void SetViewport(vk::Rect2D area);

	void Draw(const Geometry& geometry, const Pipeline& pipeline);

	template <class Push>
	void Draw(const Geometry& geometry, const Pipeline& pipeline, const Push& push)
	{
		static_assert(std::is_trivially_copyable_v<Push>, "push-константы копируются побайтово");
		DrawGeometry(geometry, pipeline, &push, static_cast<uint32_t>(sizeof(Push)));
	}

	/// Для геометрии без вершинного буфера: её строит сам вершинный шейдер
	/// по номеру вершины.
	void Draw(const Pipeline& pipeline, uint32_t vertexCount);

	template <class Push>
	void Draw(const Pipeline& pipeline, uint32_t vertexCount, const Push& push)
	{
		static_assert(std::is_trivially_copyable_v<Push>, "push-константы копируются побайтово");
		DrawProcedural(pipeline, vertexCount, &push, static_cast<uint32_t>(sizeof(Push)));
	}

private:
	void DrawGeometry(const Geometry& geometry, const Pipeline& pipeline, const void* push,
		uint32_t pushSize);
	void DrawProcedural(const Pipeline& pipeline, uint32_t vertexCount, const void* push,
		uint32_t pushSize);

	vk::CommandBuffer m_cmd;
	vk::Rect2D m_frame;
	vk::Rect2D m_viewport;
	uint32_t m_slot = 0;
};

} // namespace vkc
