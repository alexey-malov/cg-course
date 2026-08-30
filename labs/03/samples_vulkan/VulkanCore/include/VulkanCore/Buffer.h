#pragma once

#include <VulkanCore/RenderDevice.h>

namespace vkc
{

class Buffer
{
public:
	Buffer(const RenderDevice& device, vk::DeviceSize size, vk::BufferUsageFlags usage,
		MemoryKind memory = MemoryKind::DeviceLocal);

	Buffer(Buffer&&) noexcept = default;
	Buffer& operator=(Buffer&&) noexcept = default;

	/// Копирует данные в буфер. В память видеокарты — через промежуточный
	/// буфер, в отображённую — напрямую.
	void Write(const void* data, vk::DeviceSize size, vk::DeviceSize offset = 0);

	vk::Buffer Handle() const noexcept { return *m_buffer; }
	vk::DeviceSize Size() const noexcept { return m_size; }
	void* Mapped() const noexcept { return m_memory.Mapped(); }

private:
	const RenderDevice* m_device = nullptr;
	vk::raii::Buffer m_buffer{ nullptr };
	GpuMemory m_memory;
	vk::DeviceSize m_size = 0;
};

} // namespace vkc
