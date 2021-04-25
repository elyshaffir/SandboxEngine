#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace sandbox
{
	class Surface
	{
	public:
		VkSurfaceKHR surface;

		Surface(VkInstance instance, GLFWwindow * window);

		void Destroy(VkInstance instance);

	private:
		void Create(VkInstance instance, GLFWwindow * window);
	};
}
