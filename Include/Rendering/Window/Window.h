#pragma once

#include <string>
#include <memory>

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
		uint32_t width;
		uint32_t height;

		explicit Window(const WindowConfigurationInfo & configurationInfo);

		~Window();

		bool ShouldClose() const;

		bool WasResized() const {return resized;}

		void ResetResized() {resized = false;}

		void Recreate() const;

	private:
		inline static bool initialized = false;

		static void ResizeCallback(GLFWwindow * window, int width, int height);

		std::string title;
		bool resized = false;

		void Create();
	};
}
