#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

#include "Window.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Model.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "RenderSystem.hpp"

namespace MAGE {

class Engine {
public:
	void run();

	Engine();
	~Engine();

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	float m_deltaTime = 0.0f;
	float m_lastFrame = 0.0f;

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	void loadGameObjects();

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
	Device m_device{m_window};
	Renderer m_renderer{m_window, m_device};

	std::vector<GameObject> m_gameObjects;
};

} // namespace MAGE