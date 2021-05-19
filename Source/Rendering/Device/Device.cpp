#include <Rendering/Device/Device.h>

#include <set>

#include <glog/logging.h>

static bool IsDeviceExtensionsSupported(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

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
		: physicalDevice(), memoryProperties(), device(), swapChainSupport(), graphicsQueue(), presentQueue()
{
	PickPhysicalDevice(instance, surface, windowExtent);
	memoryProperties = DeviceMemoryProperties(physicalDevice);
	CreateLogicalDevice({graphicsQueue.family, presentQueue.family});
	swapChain = SwapChain(swapChainSupport, memoryProperties, device, surface, graphicsQueue.family,
						  presentQueue.family);
	graphicsQueue.Create(device, swapChainSupport.imageCount);
	presentQueue.Create(device);
}

void sandbox::Device::Destroy()
{
	graphicsQueue.Destroy(device);
	presentQueue.Destroy(device);
	swapChain.Destroy(device, false);
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

VkRenderPass sandbox::Device::GetRenderPass() const
{
	return swapChain.renderPass;
}

void sandbox::Device::AllocateVertexBuffer(VertexBuffer & vertexBuffer)
{
	BufferAllocationData allocationData = vertexBuffer.GenerateAllocationData();
	CreateBuffer(allocationData.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer.buffer,
				 allocationData.memory);

	void * data = nullptr;
	vkMapMemory(device, allocationData.memory, 0, allocationData.size, 0, &data);
	memcpy(data, allocationData.data, static_cast<size_t>(allocationData.size));
	vkUnmapMemory(device, allocationData.memory);
}

bool sandbox::Device::DrawFrame(VkPipeline pipeline, const Model & model)
{
	uint32_t imageIndex = 0;
	VkResult result = swapChain.AcquireNextImage(device, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return false;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	graphicsQueue.RecordCommandBuffers(swapChain.renderPass, imageIndex, swapChain.framebuffers[imageIndex],
									   swapChainSupport.chosenExtent, pipeline, model);
	result = swapChain.SubmitCommandBuffers(device, graphicsQueue.GetCommandBuffer(imageIndex), &imageIndex,
											graphicsQueue.queue, presentQueue.queue);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return false;
	}
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}
	return true;
}

void sandbox::Device::RecreateSwapChain(VkSurfaceKHR surface, VkExtent2D windowExtent)
{
	uint32_t oldImageCount = swapChainSupport.imageCount;
	swapChainSupport = SwapChainSupport(physicalDevice, surface, windowExtent);
	swapChain.Destroy(device, true);
	swapChain = SwapChain(swapChainSupport, memoryProperties, device, surface, graphicsQueue.family,
						  presentQueue.family, swapChain.swapChain);
	if (swapChainSupport.imageCount != oldImageCount)
	{
		graphicsQueue.FreeCommandBuffers(device);
		graphicsQueue.CreateCommandBuffers(device, swapChainSupport.imageCount);
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

void sandbox::Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
								   VkBuffer & buffer, VkDeviceMemory & bufferMemory) const
{
	VkBufferCreateInfo bufferInfo = { };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer");
	}

	VkMemoryRequirements memoryRequirements = { };
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo { };
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = memoryProperties.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory");
	}

	if (vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind buffer memory");
	}
}
