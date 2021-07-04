#include <Rendering/Device/Device.h>

#include <glog/logging.h>

static VkPhysicalDeviceFeatures GenerateRequiredDeviceFeatures()
{
	VkPhysicalDeviceFeatures deviceFeatures = { };
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	return deviceFeatures;
}

sandbox::Device::Device(VkInstance instance, VkSurfaceKHR surface, VkExtent2D windowExtent)
		: physicalDevice(instance, surface), device(), graphicsQueue(), presentQueue()
{
	CreateLogicalDevice();
	vkGetDeviceQueue(device, physicalDevice.graphicsQueueFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, physicalDevice.presentQueueFamilyIndex, 0, &presentQueue);
	swapChain = SwapChain(physicalDevice.physicalDevice, device, surface, physicalDevice.ChoosePresentMode(surface),
						  windowExtent, physicalDevice.GenerateSurfaceFormats(surface), physicalDevice.properties,
						  physicalDevice.graphicsQueueFamilyIndex, physicalDevice.presentQueueFamilyIndex);
}

void sandbox::Device::Destroy()
{
	swapChain.Destroy(device, false);
	vkDestroyDevice(device, nullptr);
}

void sandbox::Device::CreateLogicalDevice()
{
	std::set<uint32_t> uniqueQueueFamilies = {
			physicalDevice.graphicsQueueFamilyIndex, physicalDevice.presentQueueFamilyIndex
	};
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
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
	createInfo.enabledExtensionCount = static_cast<uint32_t>(PhysicalDevice::NEEDED_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = PhysicalDevice::NEEDED_EXTENSIONS.data();

	if (vkCreateDevice(physicalDevice.physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device");
	}
}

void sandbox::Device::AllocateVertexBuffer(VertexBuffer & vertexBuffer)
{
	BufferAllocationData allocationData = vertexBuffer.GenerateAllocationData();
	CreateBuffer(allocationData.size, vertexBuffer.buffer, allocationData.memory);

	void * data = nullptr;
	vkMapMemory(device, allocationData.memory, 0, allocationData.size, 0, &data);
	memcpy(data, allocationData.data, static_cast<size_t>(allocationData.size));
	vkUnmapMemory(device, allocationData.memory);
}

bool sandbox::Device::FinalizeDrawFrame()
{
	VkResult result = swapChain.FinalizeDrawFrame(device, graphicsQueue, presentQueue);
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
	swapChain.Destroy(device, true);
	swapChain = SwapChain(physicalDevice.physicalDevice, device, surface, physicalDevice.ChoosePresentMode(surface),
						  windowExtent, physicalDevice.GenerateSurfaceFormats(surface), physicalDevice.properties,
						  physicalDevice.graphicsQueueFamilyIndex, physicalDevice.presentQueueFamilyIndex,
						  swapChain.swapChain);
}

void sandbox::Device::CreateBuffer(VkDeviceSize size, VkBuffer & buffer, VkDeviceMemory & bufferMemory) const
{
	VkBufferCreateInfo bufferInfo = { };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
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
	VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
										  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (uint32_t i = 0; i < physicalDevice.properties.memoryTypeCount; i++)
	{
		if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
			(physicalDevice.properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
		{
			allocateInfo.memoryTypeIndex = i;
			if (vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate vertex buffer memory");
			}

			if (vkBindBufferMemory(device, buffer, bufferMemory, 0) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to bind buffer memory");
			}
			return;
		}
	}

	throw std::runtime_error("Failed to find suitable buffer memory type");
}
