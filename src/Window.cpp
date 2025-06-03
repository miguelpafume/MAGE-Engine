#include "Window.hpp"

namespace MAGE {
Window::Window(uint32_t width, uint32_t height, const char* title) : m_width(width), m_height(height), m_title(title) {
	if (!glfwInit()) {
		throw std::runtime_error("FAILED TO INITIALIZE GLFW!");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	initWindow();

	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

Window::~Window() {
	glfwDestroyWindow(m_window);
}

void Window::initWindow() {
	m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);

	if (!m_window) {
		glfwTerminate();
		throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
	}
}

void Window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->m_FramebufferResized = true;
}

} // namespace MAGE