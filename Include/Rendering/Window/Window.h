#ifndef SANDBOXENGINE_WINDOW_H
#define SANDBOXENGINE_WINDOW_H

#include <string>
#include <memory>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace sandbox
{
	struct WindowConfigurationInfo
	{
		uint16_t windowWidth;
		uint16_t windowHeight;
		std::string windowTitle;
	};

	class Window
	{
	public:
		GLFWwindow * window;

		explicit Window(const WindowConfigurationInfo & configurationInfo);

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
