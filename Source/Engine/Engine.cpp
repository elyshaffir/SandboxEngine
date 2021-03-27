#include <Engine/Engine.h>
#include <Engine/Debugging/Debugging.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <unordered_set>

static void AssertGLFWRequiredInstanceExtensions()
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	LOG(INFO) << "Available extensions:";
	std::unordered_set<std::string> availableExtensions;
	for (const auto & extension : extensions)
	{
		LOG(INFO) << "\t" << extension.extensionName;
		availableExtensions.insert(extension.extensionName);
	}

	LOG(INFO) << "Required extensions:";
	std::vector<const char *> requiredExtensions = sandbox::Engine::GetRequiredGLFWExtensions();
	for (const auto & required : requiredExtensions)
	{
		LOG(INFO) << "\t" << required;
		if (availableExtensions.find(required) == availableExtensions.end())
		{
			throw std::runtime_error("Missing required GLFW extension");
		}
	}
}

void sandbox::Engine::Initialize(char * argv0)
{
	Debugging::Initialize(argv0);
	AssertGLFWRequiredInstanceExtensions();
	glfwInit();
}

void sandbox::Engine::Update()
{
	glfwPollEvents();
}

void sandbox::Engine::TearDown()
{
	glfwTerminate();
}

std::vector<const char *> sandbox::Engine::GetRequiredGLFWExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ENABLE_VALIDATION_LAYERS
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return extensions;
}
