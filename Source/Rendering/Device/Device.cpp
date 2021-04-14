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
	CreateCommandBuffers();
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

void sandbox::Device::CreateCommandBuffers()
{
	swapChainSupport.ResizeCommandBuffersVector(commandBuffers);
	VkCommandBufferAllocateInfo allocateInfo = { };
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandPool = commandPool;
	allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers");
	}

	for (uint32_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = { };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = { };
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapchain_render_pass;
		renderPassBeginInfo.framebuffer = swapchain.getFrameBuffer(i);
		renderPassBeginInfo.renderArea.extent = swapchain_extent;

		std::array<VkClearValue, 2> clearValues = { };
		clearValues[0].color = {0.93f, 0.69f, 0.86f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		pipeline.Bind(commandBuffers[i]);
		vkCmdDraw(commandBuffers[0], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[0]);
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
	}
}
