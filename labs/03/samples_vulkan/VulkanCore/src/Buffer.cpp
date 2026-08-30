#include <VulkanCore/Buffer.h>

#include <cstring>
#include <stdexcept>

namespace vkc
{

Buffer::Buffer(const RenderDevice& device, vk::DeviceSize size, vk::BufferUsageFlags usage,
	MemoryKind memory)
	: m_device(&device)
	, m_size(size)
{
	if (size == 0)
		throw std::runtime_error("буфер нулевого размера");

	if (memory == MemoryKind::DeviceLocal)
		usage |= vk::BufferUsageFlagBits::eTransferDst;

	const vk::BufferCreateInfo createInfo{ .size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive };

	m_buffer = vk::raii::Buffer{ device.Raii(), createInfo };
	m_memory = device.Allocate(m_buffer.getMemoryRequirements(), memory);

	m_buffer.bindMemory(m_memory.Handle(), 0);
}

void Buffer::Write(const void* data, vk::DeviceSize size, vk::DeviceSize offset)
{
	if (size == 0)
		return;

	if (offset + size > m_size)
		throw std::runtime_error("запись за пределы буфера");

	// Отображённая память пишется напрямую; у памяти видеокарты адреса нет,
	// и данные попадают в неё копированием через промежуточный буфер.
	if (m_memory.Mapped() != nullptr)
	{
		std::memcpy(static_cast<std::byte*>(m_memory.Mapped()) + offset, data, size);
		return;
	}

	Buffer staging{ *m_device, size, vk::BufferUsageFlagBits::eTransferSrc,
		MemoryKind::HostVisible };
	std::memcpy(staging.m_memory.Mapped(), data, size);

	const vk::Buffer source = staging.Handle();
	const vk::Buffer destination = Handle();

	m_device->SubmitOnce(
		[source, destination, size, offset](vk::CommandBuffer cmd) {
			const vk::BufferCopy region{ .srcOffset = 0, .dstOffset = offset, .size = size };
			cmd.copyBuffer(source, destination, region);
		});
}

} // namespace vkc
