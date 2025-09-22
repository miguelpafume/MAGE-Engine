#include "Window.hpp"

namespace MAGE {

Window::Window(uint32_t width, uint32_t height, std::string windowName) : m_width{ width }, m_height{ height }, m_windowName{ windowName } {
	initWindow();
}

Window::~Window() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
}

} // namespace MAGE