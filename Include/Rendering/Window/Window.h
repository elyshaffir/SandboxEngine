#ifndef SANDBOXENGINE_WINDOW_H
#define SANDBOXENGINE_WINDOW_H

#include <string>
#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace sandbox
{
	class Window
	{
	public:
		GLFWwindow * window;

		Window(uint32_t width, uint32_t height, std::string title);

		~Window();

		bool ShouldClose();

		VkExtent2D GenerateExtent();

	private:
		inline static bool initialized = false;

		const uint32_t width;
		const uint32_t height;
		std::string title;

		void Create();
	};
}

#endif //SANDBOXENGINE_WINDOW_H
