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

	// ### CHIP8 ###
	Chip8 chip8 {};

	VkDescriptorSet displayDescriptorSet;

	float cycleAccumulator = 0.0f;
	float timerAccumulator = 0.0f;

	#ifdef CHIP8_MODE
		chip8.LoadROM("roms/test_opcode.ch8");

		std::array<uint8_t, chip8.VIDEO_WIDTH * chip8.VIDEO_HEIGHT * 4> pixelBuffer {};
		for (size_t i = 0; i < chip8.VIDEO_WIDTH * chip8.VIDEO_HEIGHT; i++) {
			pixelBuffer[i * 4 + 3] = 255;
		}

		auto displayTexture = std::make_unique<Texture>(m_device, pixelBuffer.data(), chip8.VIDEO_WIDTH, chip8.VIDEO_HEIGHT);

		VkDescriptorImageInfo displayImageInfo = displayTexture->descriptorInfo();
		DescriptorWriter(*textureSetLayout, *m_globalPool)
			.writeImage(0, &displayImageInfo)
			.build(displayDescriptorSet);

		// Apply texture to object
		m_gameObjects[0].m_textureDescriptorSet = displayDescriptorSet;

	#else
		m_textures.push_back(std::make_unique<Texture>(m_device, "textures/perryTheChad.jpg", VK_FILTER_LINEAR));

		VkDescriptorImageInfo imgInfo = m_textures[0]->descriptorInfo();
		VkDescriptorSet texSet;
		DescriptorWriter(*textureSetLayout, *m_globalPool)
			.writeImage(0, &imgInfo)
			.build(texSet);

		// Apply texture to object
		m_gameObjects[0].m_textureDescriptorSet = texSet;
		m_gameObjects[1].m_textureDescriptorSet = defaultTextureDescriptorSet;
	#endif

	RenderSystem renderSystem {m_device, m_renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(), textureSetLayout->getDescriptorSetLayout()};

	Camera camera {};

	#ifdef CHIP8_MODE
		float aspect = m_renderer.getAspectRatio();
		float chip8Aspect = static_cast<float>(chip8.VIDEO_WIDTH) / static_cast<float>(chip8.VIDEO_HEIGHT);
		
		float top, bottom, left, right;

		if (aspect > chip8Aspect) {
			top = -1.0f;
			bottom = 1.0f;
			left = -aspect / chip8Aspect;
			right = aspect / chip8Aspect;
		}
		else {
			top = -chip8Aspect / aspect;
			bottom = chip8Aspect / aspect;
			left = -1.0f;
			right = 1.0f;
		}

		camera.setOrtohraphicProjection(left, right, top, bottom, -1.0f, 1.0f);
	#else
		float aspect = m_renderer.getAspectRatio();                                                               
		camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
	#endif

	GameObject viewObject = GameObject::createGameObject();
	KeyboardController cameraController {};
	KeyboardController chip8Controller {};
	
	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_window.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		deltaTime = std::min(deltaTime, 0.1f);
		currentTime = newTime;

		#ifdef CHIP8_MODE
			if (aspect != m_renderer.getAspectRatio()) {
				aspect = m_renderer.getAspectRatio();

				if (aspect > chip8Aspect) {
					top = -1.0f;
					bottom = 1.0f;
					left = -aspect / chip8Aspect;
					right = aspect / chip8Aspect;
				}
				else {
					top = -chip8Aspect / aspect;
					bottom = chip8Aspect / aspect;
					left = -1.0f;
					right = 1.0f;
				}

				camera.setOrtohraphicProjection(left, right, top, bottom, -1.0f, 1.0f);
			}

			chip8Controller.updateChip8Input(m_window.getGLFWWindow(), chip8);

			cycleAccumulator += deltaTime;
			while (cycleAccumulator >= chip8.CYCLE_PERIOD) {
				chip8.Cycle();
				cycleAccumulator -= chip8.CYCLE_PERIOD;
			}

			timerAccumulator += deltaTime;
			while (timerAccumulator >= chip8.TIMER_PERIOD) {
				chip8.TickTimers();
				timerAccumulator -= chip8.TIMER_PERIOD;
			}

			chip8.getPixelsRGBA(pixelBuffer.data());
			displayTexture->update(pixelBuffer.data());
		#else
			m_gameObjects[1].m_transform.rotation.x = glm::mod(m_gameObjects[1].m_transform.rotation.x + 0.3f * deltaTime, glm::two_pi<float>());
			m_gameObjects[1].m_transform.rotation.y = glm::mod(m_gameObjects[1].m_transform.rotation.y + 0.5f * deltaTime, glm::two_pi<float>());

			cameraController.moveInPlaneXZ(m_window.getGLFWWindow(), deltaTime, viewObject);
			camera.setViewXYZ(viewObject.m_transform.translation, viewObject.m_transform.rotation);

			if (aspect != m_renderer.getAspectRatio()) {
				aspect = m_renderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1, 10.0f);
			}
		#endif

		cameraController.otherKeys(m_window.getGLFWWindow(), deltaTime, viewObject);

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
	Builder planeBuilder {};

	#ifdef CHIP8_MODE
		planeBuilder.vertices = {
			{{-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		};

		planeBuilder.indices = {0, 1, 2, 0, 2, 3};

		std::shared_ptr<Model> planeModel = std::make_shared<Model>(m_device, planeBuilder);
		
		GameObject plane = GameObject::createGameObject();
		plane.m_model = planeModel;
		m_gameObjects.push_back(std::move(plane));

	#else
		planeBuilder.vertices = {
			{{-.5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-.5f,  .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{ .5f,  .5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{ .5f, -.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		};

		planeBuilder.indices = {0, 1, 2, 0, 2, 3};

		std::shared_ptr<Model> planeModel = std::make_shared<Model>(m_device, planeBuilder);
	
		GameObject plane = GameObject::createGameObject();
		
		plane.m_model = planeModel;
		plane.m_transform.translation = {0.0f, 0.0f, 2.0f};
		plane.m_transform.rotation.z = glm::radians(45.0f);
		m_gameObjects.push_back(std::move(plane));
		
		std::shared_ptr<Model> model = createCubeModel(m_device, {0.0f, 0.0f, 0.0f});
		
		GameObject cube = GameObject::createGameObject();
		cube.m_model = model;
		cube.m_transform.translation = {0.0f, 0.0f, 2.5f};
		cube.m_transform.scale = {0.5f, 0.5f, 0.5f};
		
		m_gameObjects.push_back(std::move(cube));

	#endif
}

} // namespace MAGE