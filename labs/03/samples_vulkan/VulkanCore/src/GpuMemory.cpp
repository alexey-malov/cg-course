#include <VulkanCore/GpuMemory.h>

#include <utility>

namespace vkc
{

GpuMemory::GpuMemory(vk::raii::DeviceMemory memory, vk::DeviceSize size, void* mapped)
	: m_memory(std::move(memory))
	, m_size(size)
	, m_mapped(mapped)
{
}

} // namespace vkc
