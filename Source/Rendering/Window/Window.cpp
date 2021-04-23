#include <Rendering/Window/Window.h>

sandbox::Window::Window(const WindowConfigurationInfo & configurationInfo)
		: width(configurationInfo.windowWidth), height(configurationInfo.windowHeight),
		  title(configurationInfo.windowTitle), window(nullptr)
{
	Create();
}

sandbox::Window::~Window()
{
	glfwDestroyWindow(window);
}

bool sandbox::Window::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

VkExtent2D sandbox::Window::GenerateExtent()
{
	return {width, height};
}

void sandbox::Window::Create()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwDefaultWindowHints();
}
