#include "MageWindow.hpp"

MageWindow::MageWindow(uint32_t width, uint32_t height, const char* title) : width(width), height(height), title(title) {
	if (!glfwInit()) {
		throw std::runtime_error("FAILED TO INITIALIZE GLFW!");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	initWindow();

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void MageWindow::initWindow() {
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
	}
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

/*
	RESIZE FUNCIONS
*/

void MageWindow::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	auto window = reinterpret_cast<MageWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->framebufferResized = true;
}

void MageWindow::resetResizeFlag() {
	framebufferResized = false;
}

void MageWindow::getFramebufferSize(int* width, int* height) {
	glfwGetFramebufferSize(window, width, height);
}

bool MageWindow::wasResized() const {
	return framebufferResized;
}

/*
	CONST RETURN FUNCIONTS
*/

GLFWwindow* MageWindow::getGLFWWindow() const {
	return window;
}

uint32_t MageWindow::getWidth() const {
	return static_cast<uint32_t>(width);
}

uint32_t MageWindow::getHeight() const {
	return static_cast<uint32_t>(height);
}