#include <Rendering/Window/Window.h>

void sandbox::Window::ResizeCallback(GLFWwindow * window, int width, int height)
{
	auto * currentWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	currentWindow->width = static_cast<uint32_t>(width);
	currentWindow->height = static_cast<uint32_t>(height);
}

sandbox::Window::Window(const WindowConfigurationInfo & configurationInfo)
		: window(nullptr), width(configurationInfo.windowWidth), height(configurationInfo.windowHeight)
{
	Create(configurationInfo.windowTitle);
}

sandbox::Window::~Window()
{
	glfwDestroyWindow(window);
}

bool sandbox::Window::ShouldClose() const
{
	return glfwWindowShouldClose(window);
}

void sandbox::Window::Recreate() const
{
	uint32_t newWidth = width;
	uint32_t newHeight = height;
	while (newWidth == 0 || newHeight == 0)
	{
		newWidth = width;
		newHeight = height;
		glfwWaitEvents();
	}
}

void sandbox::Window::Create(const std::string & title)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
	glfwDefaultWindowHints();
}
