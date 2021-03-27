#include <Rendering/Surface/Surface.h>

#include <stdexcept>

sandbox::Surface::Surface(VkInstance instance, GLFWwindow * window) : surface(0)
{
	Create(instance, window);
}

void sandbox::Surface::Destroy(VkInstance instance)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

void sandbox::Surface::Create(VkInstance instance, GLFWwindow * window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}
