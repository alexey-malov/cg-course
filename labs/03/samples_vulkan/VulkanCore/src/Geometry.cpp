#include <VulkanCore/Geometry.h>

#include <stdexcept>
#include <utility>

namespace vkc
{

Geometry::Geometry(const RenderDevice& device, const void* vertices, vk::DeviceSize bytes,
	uint32_t count, VertexLayout layout, uint32_t copies)
	: m_layout(std::move(layout))
	, m_count(count)
{
	if (copies == 0)
		throw std::runtime_error("геометрия не может состоять из нуля копий");

	// Единственную копию никто не переписывает, ей подходит память видеокарты.
	// Копии на каждый кадр переписываются постоянно, поэтому лежат в памяти,
	// видимой процессору: запись в неё — обычное копирование байтов.
	const MemoryKind memory = copies == 1 ? MemoryKind::DeviceLocal : MemoryKind::HostVisible;

	m_vertexBuffers.reserve(copies);
	for (uint32_t i = 0; i < copies; ++i)
	{
		Buffer& buffer = m_vertexBuffers.emplace_back(
			device, bytes, vk::BufferUsageFlagBits::eVertexBuffer, memory);
		buffer.Write(vertices, bytes);
	}
}

void Geometry::UpdateRaw(uint32_t slot, const void* data, vk::DeviceSize bytes)
{
	if (m_vertexBuffers.size() == 1)
	{
		throw std::runtime_error(
			"геометрия создана неизменяемой; укажите число кадров в полёте, чтобы её обновлять");
	}

	Buffer& buffer = m_vertexBuffers[slot % m_vertexBuffers.size()];

	if (bytes != buffer.Size())
		throw std::runtime_error("Geometry::Update не может менять число вершин");

	buffer.Write(data, bytes);
}

void Geometry::Bind(vk::CommandBuffer cmd, uint32_t slot) const
{
	const vk::Buffer buffer = m_vertexBuffers[slot % m_vertexBuffers.size()].Handle();
	const vk::DeviceSize offset = 0;

	cmd.bindVertexBuffers(0, buffer, offset);
}

} // namespace vkc
