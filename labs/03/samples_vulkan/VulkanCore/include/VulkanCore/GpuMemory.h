#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace vkc
{

enum class MemoryKind
{
	/// Память видеокарты
	DeviceLocal,

	/// Память, отображённая в адресное пространство процессора
	HostVisible
};

class GpuMemory
{
public:
	GpuMemory() = default;
	GpuMemory(vk::raii::DeviceMemory memory, vk::DeviceSize size, void* mapped);

	GpuMemory(GpuMemory&&) noexcept = default;
	GpuMemory& operator=(GpuMemory&&) noexcept = default;

	vk::DeviceMemory Handle() const noexcept { return *m_memory; }
	vk::DeviceSize Size() const noexcept { return m_size; }

	/// Адрес отображённой памяти. Пусто, если память видеокарты.
	void* Mapped() const noexcept { return m_mapped; }

private:
	vk::raii::DeviceMemory m_memory{ nullptr };
	vk::DeviceSize m_size = 0;
	void* m_mapped = nullptr;
};

} // namespace vkc
