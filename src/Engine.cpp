#include <memory>
#include <stdexcept>
#include <array>
#include <queue>

#include <vulkan/vulkan_core.h>

#include "Engine.hpp"

namespace MAGE {

Engine::Engine() { 
	loadGameObjects();
}

Engine::~Engine() { 
}

void Engine::run() {
	RenderSystem renderSystem {m_device, m_renderer.getSwapChainRenderPass()};

	while (!m_window.shouldClose()) {
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		m_deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		if (VkCommandBuffer commandBuffer = m_renderer.beginFrame()) {
			m_renderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderGameObject(commandBuffer, m_gameObjects, m_deltaTime);
			m_renderer.endSwapChainRenderPass(commandBuffer);

			m_renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_device.getDevice());
}

void Engine::loadGameObjects() {
	std::vector<Vertex> vertices {
		{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	std::shared_ptr<Model> m_model = std::make_shared<Model>(m_device, vertices);

	GameObject triangle = GameObject::createGameObject();
	triangle.m_model = m_model;
	triangle.m_color = {0.1f, 0.8f, 0.1f};
	triangle.m_transform2d.translation.x = 0.2f;
	triangle.m_transform2d.scale = {2.0f, 0.5f};
	triangle.m_transform2d.rotation = 0.25f * glm::two_pi<float>();

	m_gameObjects.push_back(std::move(triangle));
}

} // namespace MAGE