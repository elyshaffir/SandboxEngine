#include <Rendering/Device/DeviceMemoryProperties.h>

#include <stdexcept>

sandbox::DeviceMemoryProperties::DeviceMemoryProperties() : properties()
{

}

sandbox::DeviceMemoryProperties::DeviceMemoryProperties(VkPhysicalDevice physicalDevice) : properties()
{
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);
}

uint32_t sandbox::DeviceMemoryProperties::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const
{
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}
