#include <Rendering/SwapChain/SwapChainImage.h>

#include <stdexcept>
#include <array>


sandbox::SwapChainImage::SwapChainImage(VkDevice device, VkRenderPass renderPass, VkFormat depthFormat,
										VkExtent2D swapChainExtent, VkImage swapChainImage,
										VkCommandBuffer commandBuffer,
										const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties,
										const VkSurfaceFormatKHR & surfaceFormat)
		: depthImageMemory(), depthImage(), swapChainImage(swapChainImage), depthImageView(), imageView(),
		  framebuffer(), inFlightFence(), commandBuffer(commandBuffer)
{
	CreateImageView(device, surfaceFormat);
	CreateDepthImage(device, depthFormat, swapChainExtent, physicalDeviceMemoryProperties);
	CreateDepthImageView(device, depthFormat);
	CreateFramebuffer(device, renderPass, swapChainExtent);
}

void sandbox::SwapChainImage::BeginCommandBuffer() const
{
	VkCommandBufferBeginInfo beginInfo = { };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer");
	}
}

void sandbox::SwapChainImage::EndCommandBuffer() const
{
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");
	}
}

void sandbox::SwapChainImage::Destroy(VkDevice device, VkCommandPool commandPool) const
{
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);
	vkDestroyFramebuffer(device, framebuffer, nullptr);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void sandbox::SwapChainImage::CreateImageView(VkDevice device, const VkSurfaceFormatKHR & surfaceFormat)
{
	VkImageViewCreateInfo viewCreateInfo = { };
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = swapChainImage;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = surfaceFormat.format;
	viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture image view");
	}
}

void sandbox::SwapChainImage::CreateDepthImage(VkDevice device, VkFormat depthFormat, VkExtent2D swapChainExtent,
											   const VkPhysicalDeviceMemoryProperties & physicalDeviceMemoryProperties)
{
	VkImageCreateInfo depthImageInfo = { };
	depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
	depthImageInfo.format = depthFormat;
	depthImageInfo.extent.width = swapChainExtent.width;
	depthImageInfo.extent.height = swapChainExtent.height;
	depthImageInfo.extent.depth = 1;
	depthImageInfo.mipLevels = 1;
	depthImageInfo.arrayLayers = 1;
	depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &depthImageInfo, nullptr, &depthImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create image");
	}

	VkMemoryRequirements memoryRequirements = { };
	vkGetImageMemoryRequirements(device, depthImage, &memoryRequirements);

	VkMemoryAllocateInfo allocInfo = { };
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryRequirements.size;
	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
			(physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
		{
			allocInfo.memoryTypeIndex = i;
			if (vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate image memory");
			}

			if (vkBindImageMemory(device, depthImage, depthImageMemory, 0) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to bind image memory");
			}
			return;
		}
	}

	throw std::runtime_error("Failed to find suitable image memory type");
}

void sandbox::SwapChainImage::CreateDepthImageView(VkDevice device, VkFormat depthFormat)
{
	VkImageViewCreateInfo depthImageViewInfo = { };
	depthImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthImageViewInfo.image = depthImage;
	depthImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageViewInfo.format = depthFormat;
	depthImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageViewInfo.subresourceRange.levelCount = 1;
	depthImageViewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &depthImageViewInfo, nullptr, &depthImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create depth image view");
	}
}

void sandbox::SwapChainImage::CreateFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent)
{
	std::array<VkImageView, 2> attachments = {imageView, depthImageView};

	VkFramebufferCreateInfo framebufferInfo = { };
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = swapChainExtent.width;
	framebufferInfo.height = swapChainExtent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create framebuffer");
	}
}
