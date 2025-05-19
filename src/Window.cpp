#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const char* title) : width(width), height(height), title(title) {
	if (!glfwInit()) {
		throw std::runtime_error("FAILED TO INITIALIZE GLFW!");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	initWindow();

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Window::initWindow() {
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		throw std::runtime_error("FAILED TO CREATE GLFW WINDOW!");
	}
}

Window::~Window() {
	glfwDestroyWindow(window);
}

void Window::poolEvents() {
	glfwPollEvents();
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

/*
	RESIZE FUNCIONS
*/

void Window::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
	auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
	window->framebufferResized = true;
}

void Window::resetResizeFlag() {
	framebufferResized = false;
}

void Window::getFramebufferSize(int* width, int* height) {
	glfwGetFramebufferSize(window, width, height);
}

bool Window::wasResized() const {
	return framebufferResized;
}

/*
	CONST RETURN FUNCIONTS
*/

GLFWwindow* Window::getGLFWWindow() const {
	return window;
}

uint32_t Window::getWidth() const {
	return static_cast<uint32_t>(width);
}

uint32_t Window::getHeight() const {
	return static_cast<uint32_t>(height);
}