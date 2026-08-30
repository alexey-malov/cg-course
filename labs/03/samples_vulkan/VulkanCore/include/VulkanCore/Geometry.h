#pragma once

#include <VulkanCore/Buffer.h>
#include <VulkanCore/Frame.h>
#include <VulkanCore/Vertex.h>

#include <ranges>
#include <vector>

namespace vkc
{

/// Непрерывная последовательность вершин: vector, array, span.
template <class R>
concept VertexRange = std::ranges::contiguous_range<R> && VertexType<std::ranges::range_value_t<R>>;

/// Вершины в памяти видеокарты вместе с их раскладкой.
class Geometry
{
public:
	/// По умолчанию геометрия неизменяемая: загружается один раз.
	/// Если передать число кадров в полёте, геометрия заведёт по копии вершин
	/// на кадр, и её можно будет переписывать через Update.
	template <VertexRange V>
	Geometry(const RenderDevice& device, const V& vertices, uint32_t framesInFlight = 1)
		: Geometry(device, std::ranges::data(vertices), Bytes(vertices),
			  static_cast<uint32_t>(std::ranges::size(vertices)),
			  std::ranges::range_value_t<V>::Layout(), framesInFlight)
	{
	}

	Geometry(Geometry&&) noexcept = default;
	Geometry& operator=(Geometry&&) noexcept = default;

	/// Переписывает вершины в копии текущего кадра.
	/// Число вершин меняться не должно.
	template <VertexRange V>
	void Update(const Frame& frame, const V& vertices)
	{
		UpdateRaw(frame.slot, std::ranges::data(vertices), Bytes(vertices));
	}

	void Bind(vk::CommandBuffer cmd, uint32_t slot) const;

	uint32_t Count() const noexcept { return m_count; }
	const VertexLayout& Layout() const noexcept { return m_layout; }

private:
	Geometry(const RenderDevice& device, const void* vertices, vk::DeviceSize bytes, uint32_t count,
		VertexLayout layout, uint32_t copies);

	template <VertexRange V>
	static vk::DeviceSize Bytes(const V& vertices)
	{
		return std::ranges::size(vertices) * sizeof(std::ranges::range_value_t<V>);
	}

	void UpdateRaw(uint32_t slot, const void* data, vk::DeviceSize bytes);

	std::vector<Buffer> m_vertexBuffers;
	VertexLayout m_layout;
	uint32_t m_count = 0;
};

} // namespace vkc
