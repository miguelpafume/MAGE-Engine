#pragma once

#include <string>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace MAGE {

class Window {
public:
	Window(int width, int height, std::string windowName);
	~Window();

	Window(const Window&) = delete;
	Window &operator=(const Window&) = delete;
	
	bool shouldClose() 				const { return glfwWindowShouldClose(m_window); }
	bool windowResized() 			const { return framebufferResized; }
	VkExtent2D getExtent() 			const { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
	GLFWwindow* getGLFWWindow()		const { return m_window; }
	
	void resetWindowResizedFlag()	{ framebufferResized = false; }
	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	
private:
	void initWindow();
	static void framebufferResizedCallback(GLFWwindow* window, int width, int height);

	int m_width;
	int m_height;
	const std::string m_windowName;

	bool framebufferResized = false;

	GLFWwindow* m_window;
};

} // namespace MAGE