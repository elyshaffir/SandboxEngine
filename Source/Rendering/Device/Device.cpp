#include <Rendering/Device/Device.h>

#ifdef ENABLE_VALIDATION_LAYERS

#include <Rendering/Instance/Instance.h>

#endif

#include <set>

#include <glog/logging.h>


static bool IsDeviceExtensionsSupported(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
			physicalDevice,
			nullptr,
			&extensionCount,
			availableExtensions.data());

	std::set<std::string> requiredExtensions(sandbox::Device::DEVICE_EXTENSIONS.begin(),
											 sandbox::Device::DEVICE_EXTENSIONS.end());

	for (const auto & extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

static bool IsDeviceRequiredFeaturesSupported(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceFeatures supportedFeatures = { };
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	return supportedFeatures.samplerAnisotropy;
}

static VkPhysicalDeviceFeatures GenerateRequiredDeviceFeatures()
{
	VkPhysicalDeviceFeatures deviceFeatures = { };
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	return deviceFeatures;
}

sandbox::Device::Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent)
		: physicalDevice(), physicalDeviceProperties(), device(), graphicsQueue(), presentQueue(), commandPool(),
		  queueFamilyIndices(), swapChainSupport(), swapChain()
{
	PickPhysicalDevice(instance, surface, windowExtent);
	CreateLogicalDevice();
	CreateCommandPool();
	CreateSwapChain(surface);
}

sandbox::Device::~Device()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);
}

void sandbox::Device::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}
	LOG(INFO) << "Device count: " << deviceCount;
	std::vector<VkPhysicalDevice> availableDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices.data());

	for (const auto & availableDevice : availableDevices)
	{
		QueueFamilyIndices potentialQueueFamilyIndices(availableDevice, surface);
		SwapChainSupport potentialSwapChainSupport(availableDevice, surface, windowExtent);

		if (potentialQueueFamilyIndices.IsComplete() && IsDeviceExtensionsSupported(availableDevice) &&
			potentialSwapChainSupport.IsComplete() && IsDeviceRequiredFeaturesSupported(availableDevice))
		{
			physicalDevice = availableDevice;
			queueFamilyIndices = potentialQueueFamilyIndices;
			swapChainSupport = potentialSwapChainSupport;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU");
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	LOG(INFO) << "Physical device: " << physicalDeviceProperties.deviceName;
}

void sandbox::Device::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueFamilyIndices.FillQueueCreateInfos(queueCreateInfos);

	VkPhysicalDeviceFeatures requiredDeviceFeatures = GenerateRequiredDeviceFeatures();

	VkDeviceCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &requiredDeviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

#ifdef ENABLE_VALIDATION_LAYERS
	createInfo.enabledLayerCount = static_cast<uint32_t>(Instance::VALIDATION_LAYERS.size());
	createInfo.ppEnabledLayerNames = Instance::VALIDATION_LAYERS.data();
#endif

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}

	queueFamilyIndices.GetDeviceQueues(device, &graphicsQueue, &presentQueue);
}

void sandbox::Device::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = { };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	queueFamilyIndices.PopulateGraphicsCommandPoolCreateInfo(&poolInfo);
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool");
	}
}

void sandbox::Device::CreateSwapChain(VkSurfaceKHR surface)
{
	swapChain = SwapChain(swapChainSupport, queueFamilyIndices, physicalDevice, device, surface);
}
