#include "MageWindow.hpp"

MageWindow::MageWindow(uint16_t width, uint16_t height, const char* title) : width(width), height(height), title(title) {
	if (!glfwInit()) {
		throw std::runtime_error("FAILED TO INITIALIZE GLFW!");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // ### TEMP

	initWindow();
}

MageWindow::~MageWindow() {
	glfwDestroyWindow(window);
}

void MageWindow::poolEvents() {
	glfwPollEvents();
}

bool MageWindow::shouldClose() {
	return glfwWindowShouldClose(window);
}

GLFWwindow* MageWindow::getGLFWWindow() const {
	return window;
}

uint16_t MageWindow::getWidth() const {
	return width;
}

uint16_t MageWindow::getHeight() const {
	return height;
}
void MageWindow::initWindow() {
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
	}
}