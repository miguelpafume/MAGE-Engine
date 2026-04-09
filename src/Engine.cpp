#include "Engine.hpp"

namespace MAGE {

std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
	std::vector<Vertex> vertices{
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

	};
	for (auto& v : vertices) {
		v.position += offset;
	}
	return std::make_unique<Model>(device, vertices);
}

Engine::Engine() { 
	loadGameObjects();
}

Engine::~Engine() { 
}

void Engine::run() {
	RenderSystem renderSystem {m_device, m_renderer.getSwapChainRenderPass()};
	Camera camera {};
	float aspect = m_renderer.getAspectRatio();
	camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 10.0f);
	// camera.setViewDirection(glm::vec3 {0.0f, 0.0f, 0.0f}, glm::vec3 {0.5f, 0.0f, 1.0f}, glm::vec3 {0.0f, -1.0f, 0.0f});
	camera.setViewTarget(glm::vec3{-1.0f, -2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 2.5f});

	while (!m_window.shouldClose()) {
		glfwPollEvents();

		if (aspect != m_renderer.getAspectRatio()) {
			aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 10.0f);
			// camera.setOrtohraphicProjection(-aspect, aspect, -1, 1, -1, 1);
		}

		float currentFrame = glfwGetTime();
		m_deltaTime = currentFrame - m_lastFrame;
		m_lastFrame = currentFrame;

		m_gameObjects[1].m_transform.rotation.x = glm::mod(m_gameObjects[1].m_transform.rotation.x + 0.3f * m_deltaTime, glm::two_pi<float>());
		m_gameObjects[1].m_transform.rotation.y = glm::mod(m_gameObjects[1].m_transform.rotation.y + 0.5f * m_deltaTime, glm::two_pi<float>());
		// obj.m_transform.rotation.z = glm::mod(obj.m_transform.rotation.z + 0.5f * deltaTime, glm::two_pi<float>());

		if (VkCommandBuffer commandBuffer = m_renderer.beginFrame()) {
			m_renderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderGameObject(commandBuffer, m_gameObjects, camera);
			m_renderer.endSwapChainRenderPass(commandBuffer);

			m_renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_device.getDevice());
}

void Engine::loadGameObjects() {
	std::shared_ptr<Model> model = createCubeModel(m_device, {0.0f, 0.0f, 0.0f});

	std::vector<Vertex> planeVertices {
		{{-.5f, -.5f, 0.5f},{.5f, .1f, .8f}},
		{{.5f, .5f, 0.5f},	{.5f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.5f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f},{.5f, .1f, .8f}},
		{{.5f, .5f, 0.5f},	{.5f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.5f, .1f, .8f}},
	};

	std::shared_ptr<Model> planeModel = std::make_shared<Model>(m_device, planeVertices);
	
	GameObject plane = GameObject::createGameObject();
	plane.m_model = planeModel;
	plane.m_transform.translation = {0.0f, 0.0f, 2.0f};
	plane.m_transform.scale = {1.0f, 1.0f, 1.0f};
	plane.m_transform.rotation.x = 0.0f;
	plane.m_transform.rotation.y = 0.0f;
	plane.m_transform.rotation.z = glm::radians(45.0f);
	m_gameObjects.push_back(std::move(plane));

	GameObject cube = GameObject::createGameObject();
	cube.m_model = model;
	cube.m_transform.translation = {0.0f, 0.0f, 2.5f};
	cube.m_transform.scale = {0.5f, 0.5f, 0.5f};

	m_gameObjects.push_back(std::move(cube));
}

} // namespace MAGE