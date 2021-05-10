#pragma once

#include <vulkan/vulkan.h>

namespace sandbox
{
	class DeviceMemoryProperties
	{
	public:
		DeviceMemoryProperties();

		explicit DeviceMemoryProperties(VkPhysicalDevice physicalDevice);

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;

	private:
		VkPhysicalDeviceMemoryProperties properties;
	};
}
