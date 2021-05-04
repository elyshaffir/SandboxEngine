#include <Rendering/Device/Device.h>

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
		: physicalDevice(), device(), swapChainSupport(), graphicsQueue(), presentQueue()
{
	PickPhysicalDevice(instance, surface, windowExtent);
	CreateLogicalDevice({graphicsQueue.family, presentQueue.family});
	swapChain = SwapChain(swapChainSupport, physicalDevice, device, surface, graphicsQueue.family, presentQueue.family);
	graphicsQueue.Create(device, swapChainSupport.imageCount);
	presentQueue.Create(device);
}

void sandbox::Device::Destroy()
{
	graphicsQueue.Destroy(device);
	presentQueue.Destroy(device);
	swapChain.Destroy(device);
	vkDestroyDevice(device, nullptr);
}

void sandbox::Device::CreateLogicalDevice(const std::set<uint32_t> & queueFamilies)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;

	for (uint32_t queueFamily : queueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = { };
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures requiredDeviceFeatures = GenerateRequiredDeviceFeatures();

	VkDeviceCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &requiredDeviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}
}

void sandbox::Device::RecordRenderPass(VkPipeline pipeline)
{
	graphicsQueue.RecordRenderPass(swapChain.renderPass, swapChain.framebuffers, swapChainSupport.chosenExtent,
								   pipeline);
}

void sandbox::Device::DrawFrame()
{
	uint32_t imageIndex = 0;
	VkResult result = swapChain.AcquireNextImage(device, &imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	result = swapChain.SubmitCommandBuffers(device, graphicsQueue.GetCommandBuffer(imageIndex), &imageIndex,
											graphicsQueue.queue, presentQueue.queue);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}
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
		SwapChainSupport potentialSwapChainSupport(availableDevice, surface, windowExtent);

		if (IsDeviceExtensionsSupported(availableDevice) && potentialSwapChainSupport.IsComplete() &&
			IsDeviceRequiredFeaturesSupported(availableDevice) && graphicsQueue.FindFamily(availableDevice) &&
			presentQueue.FindFamily(availableDevice, surface))
		{
			physicalDevice = availableDevice;
			swapChainSupport = potentialSwapChainSupport;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU");
	}

	VkPhysicalDeviceProperties physicalDeviceProperties = { };
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	LOG(INFO) << "Physical device: " << physicalDeviceProperties.deviceName;
}
