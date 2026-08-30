#pragma once

#include <VulkanCore/GpuMemory.h>

#include <functional>

namespace vkc
{

class Window;

/// Устройство Vulkan вместе со всем, что к нему прилагается: экземпляр,
/// поверхность, очереди, выделение памяти и одноразовые команды.
class RenderDevice
{
public:
	explicit RenderDevice(const Window& window);

	RenderDevice(const RenderDevice&) = delete;
	RenderDevice& operator=(const RenderDevice&) = delete;

	vk::Device Handle() const noexcept { return *m_device; }
	vk::PhysicalDevice PhysicalDevice() const noexcept { return *m_physicalDevice; }
	vk::Instance Instance() const noexcept { return *m_instance; }
	vk::SurfaceKHR Surface() const noexcept { return *m_surface; }
	vk::Queue GraphicsQueue() const noexcept { return *m_graphicsQueue; }
	vk::Queue PresentQueue() const noexcept { return *m_presentQueue; }
	uint32_t GraphicsFamily() const noexcept { return m_graphicsFamily; }
	uint32_t PresentFamily() const noexcept { return m_presentFamily; }

	/// Выделяет память под ресурс: одно выделение на ресурс.
	GpuMemory Allocate(vk::MemoryRequirements requirements, MemoryKind kind) const;

	/// Записывает и синхронно выполняет одноразовый набор команд.
	void SubmitOnce(const std::function<void(vk::CommandBuffer)>& record) const;

	void WaitIdle() const;

	const vk::raii::Device& Raii() const noexcept { return m_device; }

private:
	void CreateInstance();
	void CreateDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

	// Порядок объявления задаёт порядок создания. Уничтожение идёт в обратном,
	// поэтому поверхность умирает раньше экземпляра, как требует Vulkan.
	vk::raii::Context m_context;
	vk::raii::Instance m_instance{ nullptr };
	vk::raii::DebugUtilsMessengerEXT m_debugMessenger{ nullptr };
	vk::raii::SurfaceKHR m_surface{ nullptr };
	vk::raii::PhysicalDevice m_physicalDevice{ nullptr };
	vk::raii::Device m_device{ nullptr };
	vk::raii::Queue m_graphicsQueue{ nullptr };
	vk::raii::Queue m_presentQueue{ nullptr };
	vk::raii::CommandPool m_transferPool{ nullptr };

	uint32_t m_graphicsFamily = 0;
	uint32_t m_presentFamily = 0;
};

} // namespace vkc
