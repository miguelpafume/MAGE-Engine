#include "Window.hpp"

namespace MAGE {
	Window::Window(int width, int height, std::string windowName) : m_width{ width }, m_height{ height }, m_windowName{ windowName } {
		initWindow();
	}

	void Window::initWindow() {

	}
} // namespace MAGE