#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan_core.h>

#include <string>
#include <memory>
#include <vector>

#include "Window.hpp"
#include "Device.hpp"
#include "Renderer.hpp"
#include "Descriptor.hpp"

namespace MAGE {

class Engine {
public:
	void run();

	Engine(std::string windowTitle = "M.A.G.E.");
	~Engine();
	
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	
	uint32_t getWidth()		const { return WIDTH; }
	uint32_t getHeight()	const { return HEIGHT; }

	Window& getWindow() 								{ return m_window; }
	Device& getDevice() 								{ return m_device; }
	Renderer& getRenderer() 							{ return m_renderer; }
	std::unique_ptr<DescriptorPool>& getGlobalPool()	{ return m_globalPool; }
	
private:
	std::string m_windowTitle;
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;
	
	Window m_window{WIDTH, HEIGHT, m_windowTitle};
	Device m_device{m_window};
	Renderer m_renderer{m_window, m_device};
	std::unique_ptr<DescriptorPool> m_globalPool {};
};

} // namespace MAGE