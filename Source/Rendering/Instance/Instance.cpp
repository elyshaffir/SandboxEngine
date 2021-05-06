#include <Rendering/Instance/Instance.h>
#include <Engine/Engine.h>

#ifdef ENABLE_VALIDATION_LAYERS

#include <glog/logging.h>

static bool ValidationLayersSupported()
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char * layerName : sandbox::Instance::VALIDATION_LAYERS)
	{
		bool layerFound = false;

		for (const auto & layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
		[[maybe_unused]] void * userData)
{
	LOG(INFO) << "Validation Layer: " << callbackData->pMessage;
	return VK_FALSE;
}

static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}
#else
#include <stdexcept>
#endif

sandbox::Instance::Instance() : instance()
#ifdef ENABLE_VALIDATION_LAYERS
	, debugMessenger()
#endif
{
	Create();
#ifdef ENABLE_VALIDATION_LAYERS
	CreateDebugMessenger();
#endif
}

sandbox::Instance::~Instance()
{
#ifdef ENABLE_VALIDATION_LAYERS
	DestroyDebugMessenger();
#endif
	vkDestroyInstance(instance, nullptr);
}

#ifdef ENABLE_VALIDATION_LAYERS

void sandbox::Instance::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo = { };
	PopulateDebugMessengerCreateInfo(createInfo);
	auto creationFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
																					   "vkCreateDebugUtilsMessengerEXT");
	VkResult creationResult;
	if (creationFunction != nullptr)
	{
		creationResult = creationFunction(instance, &createInfo, nullptr, &debugMessenger);
	}
	else
	{
		creationResult = VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	if (creationResult != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger");
	}
}

void sandbox::Instance::DestroyDebugMessenger()
{
	auto destroyFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (destroyFunction != nullptr)
	{
		destroyFunction(instance, debugMessenger, nullptr);
	}
}

#endif

void sandbox::Instance::Create()
{
#ifdef ENABLE_VALIDATION_LAYERS
	if (!ValidationLayersSupported())
	{
		throw std::runtime_error("Validation layers not supported");
	}
#endif

	VkApplicationInfo appInfo = { };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = nullptr;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName = ENGINE_NAME;
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char *> extensions = Engine::GetRequiredGLFWExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef ENABLE_VALIDATION_LAYERS
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { };
	createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
	createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

	PopulateDebugMessengerCreateInfo(debugCreateInfo);
	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
#endif

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance");
	}
}

