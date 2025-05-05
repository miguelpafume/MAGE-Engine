#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

class MageWindow {
public:
	MageWindow(uint16_t width, uint16_t height, const char* title);
	~MageWindow();

	void poolEvents();
	bool shouldClose();

	GLFWwindow* getGLFWWindow() const;
	uint16_t getWidth() const;
	uint16_t getHeight() const;

private:
	GLFWwindow* window;
	uint16_t width;
	uint16_t height;
	const char* title;
	
	void initWindow();
};