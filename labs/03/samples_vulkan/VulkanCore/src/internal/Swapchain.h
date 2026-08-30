#pragma once

#include <VulkanCore/RenderDevice.h>

#include <vector>

namespace vkc
{

/// Цепочка показа. Про оконную библиотеку не знает ничего — принимает
/// размер в пикселях, а не окно.
class Swapchain
{
public:
	Swapchain(const RenderDevice& device, vk::Extent2D windowSize);

	void Recreate(vk::Extent2D windowSize);

	vk::SwapchainKHR Handle() const noexcept { return *m_swapchain; }
	vk::Format Format() const noexcept { return m_format; }
	vk::Extent2D Extent() const noexcept { return m_extent; }
	uint32_t ImageCount() const noexcept { return static_cast<uint32_t>(m_images.size()); }

	vk::Image Image(uint32_t index) const { return m_images[index]; }
	vk::ImageView View(uint32_t index) const { return *m_views[index]; }

private:
	void Create(vk::Extent2D windowSize);

	const RenderDevice* m_device = nullptr;
	vk::raii::SwapchainKHR m_swapchain{ nullptr };
	std::vector<vk::Image> m_images;
	std::vector<vk::raii::ImageView> m_views;
	vk::Format m_format = vk::Format::eUndefined;
	vk::Extent2D m_extent{};
};

} // namespace vkc
