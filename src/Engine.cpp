#include "Engine.hpp"

namespace MAGE {

std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
	Builder modelBuilder {};

	modelBuilder.vertices = {
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}, {0.0f, 0.0f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}, {0.0f, 0.0f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}, {0.0f, 0.0f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}, {0.0f, 0.0f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}, {0.0f, 0.0f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}, {0.0f, 0.0f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}, {0.0f, 0.0f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}, {0.0f, 0.0f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}, {0.0f, 0.0f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}, {0.0f, 0.0f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}, {0.0f, 0.0f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}, {0.0f, 0.0f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}, {0.0f, 0.0f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}, {0.0f, 0.0f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}, {0.0f, 0.0f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}, {0.0f, 0.0f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}, {0.0f, 0.0f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}, {0.0f, 0.0f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}, {0.0f, 0.0f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}, {0.0f, 0.0f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}, {0.0f, 0.0f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}, {0.0f, 0.0f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}, {0.0f, 0.0f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}, {0.0f, 0.0f}},
	};

	for (auto& v : modelBuilder.vertices) {
		v.position += offset;
	}

	modelBuilder.indices = {
		0, 1, 2, 0, 3, 1, // left face
		4, 6, 5, 4, 7, 5, // right face
		8, 9, 10, 8, 11, 9, // top face
		12, 13, 14, 12, 15, 13, // bottom face
		16, 17, 18, 16, 19, 17, // nose face
		20, 21, 22, 20, 23, 21 // tail face
	};

	return std::make_unique<Model>(device, modelBuilder);
}

Engine::Engine() { 
	m_globalPool = DescriptorPool::Builder(m_device)
		.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT + 10)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT) // UBOs
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10) // Texture samplers (max 10)
		.build();
	
	loadGameObjects();
}

Engine::~Engine() {}

void Engine::run() {
	std::vector<std::unique_ptr<Buffer>> uboBuffers{SwapChain::MAX_FRAMES_IN_FLIGHT};

	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<Buffer>(
			m_device,
			sizeof(GlobalUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_device.properties.limits.minUniformBufferOffsetAlignment
		);

		// ### TODO to use non-coherent memory need to fix Buffer::flush() to round offset/size to nonCoherentAtomSize boundaries.

		uboBuffers[i]->map();
	}

	// Global descriptor/set
	std::unique_ptr<DescriptorSetLayout> globalSetLayout = DescriptorSetLayout::Builder(m_device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
		
	std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
	
	for (int i = 0; i < globalDescriptorSets.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
		
		DescriptorWriter(*globalSetLayout, *m_globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}

	// Add textures
	m_textures.push_back(std::make_unique<Texture>(m_device, "textures/perryTheChad.jpg"));

	// Texture descriptor/set
	std::unique_ptr<DescriptorSetLayout> textureSetLayout = DescriptorSetLayout::Builder(m_device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
		
	std::vector<VkDescriptorSet> textureDescriptorSets(m_textures.size());

	for (int i = 0; i < m_textures.size(); i++) {
		VkDescriptorImageInfo imageInfo = m_textures[i]->descriptorInfo();

		DescriptorWriter(*textureSetLayout, *m_globalPool)
			.writeImage(0, &imageInfo)
			.build(textureDescriptorSets[i]);
	}

	// Default white texture
	m_defaultTexture = Texture::createDefaultWhite(m_device);
	VkDescriptorImageInfo defaultImageInfo = m_defaultTexture->descriptorInfo();

	VkDescriptorSet defaultTextureDescriptorSet;
	DescriptorWriter(*textureSetLayout, *m_globalPool)
		.writeImage(0, &defaultImageInfo)
		.build(defaultTextureDescriptorSet);

	// Apply texture to object
	m_gameObjects[0].m_textureDescriptorSet = textureDescriptorSets[0];
	m_gameObjects[1].m_textureDescriptorSet = defaultTextureDescriptorSet;

	RenderSystem renderSystem {m_device, m_renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout()};

	Camera camera {};
	float aspect = m_renderer.getAspectRatio();
	camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 10.0f);

	GameObject viewObject = GameObject::createGameObject();
	KeyboardController cameraController {};
	
	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_window.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		deltaTime = std::min(deltaTime, 0.1f);

		cameraController.moveInPlaneXZ(m_window.getGLFWWindow(), deltaTime, viewObject);
		cameraController.otherKeys(m_window.getGLFWWindow(), deltaTime, viewObject);
		camera.setViewXYZ(viewObject.m_transform.translation, viewObject.m_transform.rotation);

		if (aspect != m_renderer.getAspectRatio()) {
			aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 10.0f);
		}

		m_gameObjects[1].m_transform.rotation.x = glm::mod(m_gameObjects[1].m_transform.rotation.x + 0.3f * deltaTime, glm::two_pi<float>());
		m_gameObjects[1].m_transform.rotation.y = glm::mod(m_gameObjects[1].m_transform.rotation.y + 0.5f * deltaTime, glm::two_pi<float>());
		// obj.m_transform.rotation.z = glm::mod(obj.m_transform.rotation.z + 0.5f * deltaTime, glm::two_pi<float>());

		if (VkCommandBuffer commandBuffer = m_renderer.beginFrame()) {
			int frameIndex = m_renderer.getFrameIndex();
			FrameInfo frameInfo {
				.frameIndex = frameIndex,
				.deltaTime = deltaTime,
				.commandBuffer = commandBuffer,
				.camera = camera,
				.globalDescriptorSet = globalDescriptorSets[frameIndex]
			};

			// Update
			GlobalUbo ubo {};
			ubo.projectionView = camera.getProjection() * camera.getView();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);

			// Render
			m_renderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.renderGameObject(frameInfo, m_gameObjects);
			m_renderer.endSwapChainRenderPass(commandBuffer);

			m_renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_device.getDevice());
}

void Engine::loadGameObjects() {
	std::shared_ptr<Model> model = createCubeModel(m_device, {0.0f, 0.0f, 0.0f});

	Builder planeBuilder {};

	planeBuilder.vertices = {
		{{-.5f, -.5f, 0.5f},{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-.5f, .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		{{.5f, .5f, 0.5f},	{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{.5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
	};

	planeBuilder.indices = {0, 1, 2, 0, 2, 3};

	std::shared_ptr<Model> planeModel = std::make_shared<Model>(m_device, planeBuilder);
	
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