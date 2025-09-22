#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace MAGE {

class Window {
public:
	Window(int width, int height, std::string windowName);
	~Window();

private:
	void initWindow();

	const int m_width;
	const int m_height;
	const std::string m_windowName;

	GLFWwindow* window;
};

} // namespace MAGE