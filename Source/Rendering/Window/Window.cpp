#include <Rendering/Window/Window.h>
#include <stdexcept>

sandbox::Window::Window(uint32_t width, uint32_t height, std::string title) :
		width(width), height(height), title(std::move(title)), window(nullptr)
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
