#include <Rendering/Device/PhysicalDevice.h>

#include <set>

#include <glog/logging.h>

sandbox::PhysicalDevice::PhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
		: physicalDevice(), properties(), graphicsQueueFamilyIndex(), presentQueueFamilyIndex()
{
	PickPhysicalDevice(instance, surface);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);
}

std::vector<VkSurfaceFormatKHR> sandbox::PhysicalDevice::GenerateSurfaceFormats(VkSurfaceKHR surface) const
{
	std::vector<VkSurfaceFormatKHR> formats;
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}
	return formats;
}

VkPresentModeKHR sandbox::PhysicalDevice::ChoosePresentMode(VkSurfaceKHR surface) const
{
	std::vector<VkPresentModeKHR> presentModes;
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}
	else
	{
		return VK_PRESENT_MODE_MAX_ENUM_KHR;
	}

	for (const auto & presentMode : presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			LOG(INFO) << "Present mode: Mailbox";
			return presentMode;
		}
	}

	LOG(INFO) << "Present mode: V-Sync";
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkFormat sandbox::PhysicalDevice::ChooseDepthFormat() const
{
	std::vector<VkFormat> candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
	};
	for (VkFormat format : candidates)
	{
		VkFormatProperties formatProperties = { };
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

		if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) ==
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			return format;
		}
	}
	throw std::runtime_error("Failed to find a supported depth format");
}

void sandbox::PhysicalDevice::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
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
		physicalDevice = availableDevice;
		if (AreExtensionsSupported() && !GenerateSurfaceFormats(surface).empty() &&
			ChoosePresentMode(surface) != VK_PRESENT_MODE_MAX_ENUM_KHR && AreFeaturesSupported() &&
			FindGraphicsFamily() && FindPresentFamily(surface))
		{
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

bool sandbox::PhysicalDevice::AreExtensionsSupported() const
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(NEEDED_EXTENSIONS.begin(), NEEDED_EXTENSIONS.end());

	for (const auto & extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool sandbox::PhysicalDevice::AreFeaturesSupported() const
{
	VkPhysicalDeviceFeatures supportedFeatures = { };
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	return supportedFeatures.samplerAnisotropy;
}

std::vector<VkQueueFamilyProperties> sandbox::PhysicalDevice::GetQueueFamilies() const
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	return queueFamilies;
}

bool sandbox::PhysicalDevice::FindGraphicsFamily()
{
	uint32_t i = 0;
	for (const auto & queueFamily : GetQueueFamilies())
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueFamilyIndex = i;
			return true;
		}
		i++;
	}
	return false;
}

bool sandbox::PhysicalDevice::FindPresentFamily(VkSurfaceKHR surface)
{
	uint32_t i = 0;
	for (const auto & queueFamily : GetQueueFamilies())
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			presentQueueFamilyIndex = i;
			return true;
		}
		i++;
	}
	return false;
}
