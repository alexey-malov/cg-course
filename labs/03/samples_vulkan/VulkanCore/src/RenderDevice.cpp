#include <VulkanCore/RenderDevice.h>
#include <VulkanCore/Window.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace vkc
{

namespace
{

#ifdef NDEBUG
constexpr bool kEnableValidation = false;
#else
constexpr bool kEnableValidation = true;
#endif

constexpr const char* kValidationLayer = "VK_LAYER_KHRONOS_validation";

constexpr std::array kDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	vk::DebugUtilsMessageTypeFlagsEXT,
	const vk::DebugUtilsMessengerCallbackDataEXT* data,
	void*)
{
	if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
		std::fprintf(stderr, "[vulkan] %s\n", data->pMessage);

	return vk::False;
}

bool HasLayer(const vk::raii::Context& context, std::string_view name)
{
	for (const vk::LayerProperties& layer : context.enumerateInstanceLayerProperties())
	{
		if (std::string_view{ layer.layerName } == name)
			return true;
	}
	return false;
}

bool HasInstanceExtension(const vk::raii::Context& context, std::string_view name)
{
	for (const vk::ExtensionProperties& extension : context.enumerateInstanceExtensionProperties())
	{
		if (std::string_view{ extension.extensionName } == name)
			return true;
	}
	return false;
}

struct QueueFamilies
{
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> present;

	bool Complete() const { return graphics.has_value() && present.has_value(); }
};

QueueFamilies FindQueueFamilies(const vk::raii::PhysicalDevice& device, vk::SurfaceKHR surface)
{
	QueueFamilies families;

	const auto properties = device.getQueueFamilyProperties();
	for (uint32_t i = 0; i < properties.size(); ++i)
	{
		if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
			families.graphics = i;

		if (device.getSurfaceSupportKHR(i, surface) != 0u)
			families.present = i;

		if (families.Complete())
			break;
	}

	return families;
}

bool SupportsExtensions(const vk::raii::PhysicalDevice& device)
{
	const auto available = device.enumerateDeviceExtensionProperties();

	for (const char* required : kDeviceExtensions)
	{
		const bool found = std::ranges::any_of(
			available, [required](const auto& extension) { return std::string_view{ extension.extensionName } == required; });
		if (!found)
			return false;
	}
	return true;
}

} // namespace

RenderDevice::RenderDevice(const Window& window)
{
	CreateInstance();
	CreateDebugMessenger();

	m_surface = vk::raii::SurfaceKHR{ m_instance,
		static_cast<VkSurfaceKHR>(window.CreateSurface(*m_instance)) };

	PickPhysicalDevice();
	CreateLogicalDevice();

	const vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eTransient,
		.queueFamilyIndex = m_graphicsFamily };
	m_transferPool = vk::raii::CommandPool{ m_device, poolInfo };
}

void RenderDevice::CreateInstance()
{
	const vk::ApplicationInfo appInfo{ .pApplicationName = "VulkanCourse",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "VulkanCore",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3 };

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	if (glfwExtensions == nullptr)
		throw std::runtime_error("GLFW не сообщил список расширений экземпляра");

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	std::vector<const char*> layers;

	const bool useValidation = kEnableValidation && HasLayer(m_context, kValidationLayer);
	if (useValidation)
	{
		layers.push_back(kValidationLayer);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Слой проверки настраивается через VK_EXT_layer_settings — так параметры
	// задаются прямо из кода, а не переменными окружения или отдельной утилитой.
	// Это современная замена устаревшему VK_EXT_validation_features.
	//
	// Проверка синхронизации ловит гонки между записью данных процессором
	// и чтением их видеокартой — тот класс ошибок, который иначе проявляется
	// как редкое мерцание и не воспроизводится.
	const bool useLayerSettings = useValidation && HasInstanceExtension(m_context, VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);

	constexpr vk::Bool32 enabled = vk::True;
	const std::array layerSettings{
		vk::LayerSettingEXT{ .pLayerName = kValidationLayer,
			.pSettingName = "validate_sync",
			.type = vk::LayerSettingTypeEXT::eBool32,
			.valueCount = 1,
			.pValues = &enabled },
	};

	const vk::LayerSettingsCreateInfoEXT settingsInfo{ .settingCount = static_cast<uint32_t>(
														   layerSettings.size()),
		.pSettings = layerSettings.data() };

	if (useLayerSettings)
		extensions.push_back(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);

	const vk::InstanceCreateInfo createInfo{ .pNext = useLayerSettings ? &settingsInfo : nullptr,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data() };

	m_instance = vk::raii::Instance{ m_context, createInfo };
}

void RenderDevice::CreateDebugMessenger()
{
	if constexpr (kEnableValidation)
	{
		if (!HasLayer(m_context, kValidationLayer))
			return;

		const vk::DebugUtilsMessengerCreateInfoEXT createInfo{
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
				| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
				| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			.pfnUserCallback = DebugCallback
		};

		m_debugMessenger = vk::raii::DebugUtilsMessengerEXT{ m_instance, createInfo };
	}
}

void RenderDevice::PickPhysicalDevice()
{
	vk::raii::PhysicalDevices devices{ m_instance };
	if (devices.empty())
		throw std::runtime_error("не найдено ни одного устройства с поддержкой Vulkan");

	const vk::raii::PhysicalDevice* best = nullptr;
	int bestScore = -1;

	for (const vk::raii::PhysicalDevice& device : devices)
	{
		if (!SupportsExtensions(device))
			continue;

		if (!FindQueueFamilies(device, *m_surface).Complete())
			continue;

		if (device.getSurfaceFormatsKHR(*m_surface).empty())
			continue;
		if (device.getSurfacePresentModesKHR(*m_surface).empty())
			continue;

		const vk::PhysicalDeviceProperties properties = device.getProperties();
		const int score = properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? 3
			: properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu			? 2
																						: 1;

		if (score > bestScore)
		{
			bestScore = score;
			best = &device;
		}
	}

	if (best == nullptr)
		throw std::runtime_error("нет устройства, пригодного для вывода на экран");

	m_physicalDevice = *best;

	const QueueFamilies families = FindQueueFamilies(m_physicalDevice, *m_surface);
	m_graphicsFamily = *families.graphics;
	m_presentFamily = *families.present;
}

void RenderDevice::CreateLogicalDevice()
{
	const std::set<uint32_t> uniqueFamilies{ m_graphicsFamily, m_presentFamily };

	constexpr float priority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	queueInfos.reserve(uniqueFamilies.size());

	for (uint32_t family : uniqueFamilies)
	{
		queueInfos.push_back(vk::DeviceQueueCreateInfo{
			.queueFamilyIndex = family, .queueCount = 1, .pQueuePriorities = &priority });
	}

	vk::PhysicalDeviceVulkan13Features features13{ .shaderDemoteToHelperInvocation = vk::True,
		.synchronization2 = vk::True,
		.dynamicRendering = vk::True };

	// fillModeNonSolid разрешает показывать треугольники рёбрами и точками
	// вместо заливки, largePoints — задавать размер точки больше единицы.
	vk::PhysicalDeviceFeatures2 features2{
		.pNext = &features13,
		.features = { .fillModeNonSolid = vk::True, .largePoints = vk::True }
	};

	const vk::DeviceCreateInfo createInfo{ .pNext = &features2,
		.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size()),
		.pQueueCreateInfos = queueInfos.data(),
		.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensions.size()),
		.ppEnabledExtensionNames = kDeviceExtensions.data() };

	m_device = vk::raii::Device{ m_physicalDevice, createInfo };
	m_graphicsQueue = vk::raii::Queue{ m_device, m_graphicsFamily, 0 };
	m_presentQueue = vk::raii::Queue{ m_device, m_presentFamily, 0 };
}

uint32_t RenderDevice::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	const vk::PhysicalDeviceMemoryProperties available = m_physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < available.memoryTypeCount; ++i)
	{
		const bool suitableType = (typeFilter & (1u << i)) != 0u;
		const bool hasProperties = (available.memoryTypes[i].propertyFlags & properties) == properties;

		if (suitableType && hasProperties)
			return i;
	}

	throw std::runtime_error("подходящий тип памяти не найден");
}

GpuMemory RenderDevice::Allocate(vk::MemoryRequirements requirements, MemoryKind kind) const
{
	const vk::MemoryPropertyFlags properties = kind == MemoryKind::HostVisible
		? vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		: vk::MemoryPropertyFlagBits::eDeviceLocal;

	const vk::MemoryAllocateInfo allocateInfo{ .allocationSize = requirements.size,
		.memoryTypeIndex = FindMemoryType(
			requirements.memoryTypeBits, properties) };

	vk::raii::DeviceMemory memory{ m_device, allocateInfo };

	void* mapped = nullptr;
	if (kind == MemoryKind::HostVisible)
		mapped = memory.mapMemory(0, requirements.size);

	return GpuMemory{ std::move(memory), requirements.size, mapped };
}

void RenderDevice::SubmitOnce(const std::function<void(vk::CommandBuffer)>& record) const
{
	const vk::CommandBufferAllocateInfo allocateInfo{ .commandPool = *m_transferPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1 };

	vk::raii::CommandBuffers buffers{ m_device, allocateInfo };
	const vk::raii::CommandBuffer& cmd = buffers.front();

	cmd.begin(
		vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	record(*cmd);
	cmd.end();

	const vk::CommandBuffer raw = *cmd;
	const vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &raw };

	const vk::raii::Fence fence{ m_device, vk::FenceCreateInfo{} };
	m_graphicsQueue.submit(submitInfo, *fence);

	const vk::Result result = m_device.waitForFences(*fence, vk::True, std::numeric_limits<uint64_t>::max());
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("не дождались выполнения одноразовых команд");
}

void RenderDevice::WaitIdle() const
{
	m_device.waitIdle();
}

} // namespace vkc
