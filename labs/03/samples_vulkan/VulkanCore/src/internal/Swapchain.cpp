#include "Swapchain.h"

#include <algorithm>
#include <stdexcept>

namespace vkc
{

namespace
{

vk::SurfaceFormatKHR ChooseFormat(const std::vector<vk::SurfaceFormatKHR>& available)
{
	for (const vk::SurfaceFormatKHR& format : available)
	{
		if (format.format == vk::Format::eB8G8R8A8Srgb
			&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
	}
	return available.front();
}

vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& available)
{
	const bool hasMailbox = std::ranges::find(available, vk::PresentModeKHR::eMailbox) != available.end();
	return hasMailbox ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D ChooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, vk::Extent2D windowSize)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	return vk::Extent2D{ .width = std::clamp(windowSize.width, capabilities.minImageExtent.width,
							 capabilities.maxImageExtent.width),
		.height = std::clamp(windowSize.height, capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height) };
}

} // namespace

Swapchain::Swapchain(const RenderDevice& device, vk::Extent2D windowSize)
	: m_device(&device)
{
	Create(windowSize);
}

void Swapchain::Recreate(vk::Extent2D windowSize)
{
	m_device->WaitIdle();
	Create(windowSize);
}

void Swapchain::Create(vk::Extent2D windowSize)
{
	const vk::PhysicalDevice physical = m_device->PhysicalDevice();
	const vk::SurfaceKHR surface = m_device->Surface();

	const vk::SurfaceCapabilitiesKHR capabilities = physical.getSurfaceCapabilitiesKHR(surface);
	const vk::SurfaceFormatKHR format = ChooseFormat(physical.getSurfaceFormatsKHR(surface));
	const vk::PresentModeKHR mode = ChoosePresentMode(physical.getSurfacePresentModesKHR(surface));
	const vk::Extent2D extent = ChooseExtent(capabilities, windowSize);

	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0)
		imageCount = std::min(imageCount, capabilities.maxImageCount);

	const uint32_t families[] = { m_device->GraphicsFamily(), m_device->PresentFamily() };
	const bool shared = families[0] != families[1];

	const vk::SwapchainCreateInfoKHR createInfo{
		.surface = surface,
		.minImageCount = imageCount,
		.imageFormat = format.format,
		.imageColorSpace = format.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = shared ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = shared ? 2u : 0u,
		.pQueueFamilyIndices = shared ? families : nullptr,
		.preTransform = capabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = mode,
		.clipped = vk::True,
		.oldSwapchain = *m_swapchain
	};

	// Новая цепочка создаётся раньше, чем разрушается старая: так драйвер
	// может переиспользовать её ресурсы.
	vk::raii::SwapchainKHR created{ m_device->Raii(), createInfo };

	m_views.clear();
	m_swapchain = std::move(created);

	m_images = m_swapchain.getImages();
	m_format = format.format;
	m_extent = extent;

	m_views.reserve(m_images.size());
	for (vk::Image image : m_images)
	{
		const vk::ImageViewCreateInfo viewInfo{ .image = image,
			.viewType = vk::ImageViewType::e2D,
			.format = m_format,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1 } };
		m_views.emplace_back(m_device->Raii(), viewInfo);
	}
}

} // namespace vkc
