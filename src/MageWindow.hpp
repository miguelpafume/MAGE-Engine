#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class MageWindow {
public:
	MageWindow(uint32_t width, uint32_t height, const char* title);
	~MageWindow();

	MageWindow(const MageWindow&) = delete;
	MageWindow& operator=(const MageWindow&) = delete;

	void poolEvents();
	bool shouldClose();

	// RESIZE WINDOW
	bool wasResized() const;
	void resetResizeFlag();
	void getFramebufferSize(int* width, int* height);

	GLFWwindow* getGLFWWindow() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;

private:
	GLFWwindow* window;
	uint32_t width;
	uint32_t height;
	const char* title;

	// RESIZE WINDOW
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	bool framebufferResized = false;
	
	void initWindow();
};
