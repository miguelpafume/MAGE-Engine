#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace MAGE {

class Window {
public:
	Window(uint32_t width, uint32_t height, std::string windowName);
	~Window();

	Window(const Window&) = delete;
	Window &operator=(const Window&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(m_window); }

private:
	void initWindow();

	const uint32_t m_width;
	const uint32_t m_height;
	const std::string m_windowName;

	GLFWwindow* m_window;
};

} // namespace MAGE