#include <Rendering/Window/Window.h>

void sandbox::Window::ResizeCallback(GLFWwindow * window, int width, int height)
{
	auto currentWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	currentWindow->resized = true;
	currentWindow->extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

sandbox::Window::Window(const WindowConfigurationInfo & configurationInfo)
		: window(nullptr), extent({configurationInfo.windowWidth, configurationInfo.windowHeight}),
		  title(configurationInfo.windowTitle)
{
	Create();
}

sandbox::Window::~Window()
{
	glfwDestroyWindow(window);
}

bool sandbox::Window::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}

void sandbox::Window::Create()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(static_cast<int>(extent.width), static_cast<int>(extent.height), title.c_str(), nullptr,
							  nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
	glfwDefaultWindowHints();
}
