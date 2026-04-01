#include "Window.hpp"

namespace MAGE {

Window::Window(int width, int height, std::string windowName) : m_width{ width }, m_height{ height }, m_windowName{ windowName } {
	initWindow();
}

Window::~Window() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
	if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE!");
	}
}

void Window::framebufferResizedCallback(GLFWwindow* window, int width, int height) {
	Window* mageWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

	mageWindow->framebufferResized = true;
	mageWindow->m_width = width;
	mageWindow->m_height = height;
}

void Window::initWindow()
{
    glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizedCallback);
}

} // namespace MAGE