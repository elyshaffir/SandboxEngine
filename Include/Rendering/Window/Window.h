#ifndef SANDBOXENGINE_WINDOW_H
#define SANDBOXENGINE_WINDOW_H

#include <string>
#include <memory>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

namespace sandbox
{
	class Window
	{
	public:
		GLFWwindow * window;

		Window(uint16_t width, uint16_t height, std::string title);

		~Window();

		bool ShouldClose();

	private:
		inline static bool initialized = false;

		const uint16_t width;
		const uint16_t height;
		std::string title;

		void Create();
	};
}

#endif //SANDBOXENGINE_WINDOW_H
