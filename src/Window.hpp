#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace MAGE {
class Window {
public:
	Window(uint32_t width, uint32_t height, const char* title);
	~Window();

	void poolEvents() { glfwPollEvents(); }
	bool shouldClose() { return glfwWindowShouldClose(m_window); }

	bool wasResized() const { return m_FramebufferResized; }
	void resetResizeFlag() { m_FramebufferResized = false; }
	void getFramebufferSize(int* width, int* height) { glfwGetFramebufferSize(m_window, width, height); }

	GLFWwindow* getGLFWWindow() const { return m_window; }
	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }

private:
	GLFWwindow* m_window;
	uint32_t m_width;
	uint32_t m_height;
	const char* m_title;

	bool m_FramebufferResized = false;
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void initWindow();
};
} // namespace MAGE
