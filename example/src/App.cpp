#include "App.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "Buffer.hpp"
#include "Camera.hpp"
#include "Descriptor.hpp"
#include "KeyboardController.hpp"
#include "Model.hpp"
#include "RenderSystem.hpp"
#include "SwapChain.hpp"
#include "Util.hpp"

using namespace MAGE;

static std::unique_ptr<Model> createCubeModel (Device& device, glm::vec3 offset) {
    Builder cubeBuilder {};
    cubeBuilder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}, {0.f, 0.f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}, {0.f, 0.f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}, {0.f, 0.f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}, {0.f, 0.f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}, {0.f, 0.f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}, {0.f, 0.f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}, {0.f, 0.f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}, {0.f, 0.f}},

        // top face (orange — y points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}, {0.f, 0.f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}, {0.f, 0.f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}, {0.f, 0.f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}, {0.f, 0.f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}, {0.f, 0.f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}, {0.f, 0.f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}, {0.f, 0.f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}, {0.f, 0.f}},

        // nose face (blue)
        {{-.5f, -.5f, .5f}, {.1f, .1f, .8f}, {0.f, 0.f}},
        {{.5f, .5f, .5f}, {.1f, .1f, .8f}, {0.f, 0.f}},
        {{-.5f, .5f, .5f}, {.1f, .1f, .8f}, {0.f, 0.f}},
        {{.5f, -.5f, .5f}, {.1f, .1f, .8f}, {0.f, 0.f}},

        // tail face (green)
        {{-.5f, -.5f, -.5f}, {.1f, .8f, .1f}, {0.f, 0.f}},
        {{.5f, .5f, -.5f}, {.1f, .8f, .1f}, {0.f, 0.f}},
        {{-.5f, .5f, -.5f}, {.1f, .8f, .1f}, {0.f, 0.f}},
        {{.5f, -.5f, -.5f}, {.1f, .8f, .1f}, {0.f, 0.f}},
    };

    for ( Vertex& v : cubeBuilder.vertices )
        v.position += offset;

    cubeBuilder.indices = {
        0,  1,  2,  0,  3,  1,  4,  6,  5,  4,  7,  5,  8,  9,  10, 8,  11, 9,
        12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21,
    };

    return std::make_unique<Model>(device, cubeBuilder);
}

void CubeApp::loadGameObjects () {
    Device& device = m_engine.getDevice();

    std::shared_ptr<Model> cubeModel = createCubeModel(device, {0.f, 0.f, 0.f});

    Builder planeBuilder {};

    planeBuilder.vertices = {
        {{-.5f, -.5f, .5f}, {1.f, 1.f, 1.f}, {0.f, 0.f}},
        {{-.5f, .5f, .5f}, {1.f, 1.f, 1.f}, {0.f, 1.f}},
        {{.5f, .5f, .5f}, {1.f, 1.f, 1.f}, {1.f, 1.f}},
        {{.5f, -.5f, .5f}, {1.f, 1.f, 1.f}, {1.f, 0.f}},
    };

    planeBuilder.indices = {0, 1, 2, 0, 2, 3};
    std::shared_ptr<Model> planeModel = std::make_shared<Model>(device, planeBuilder);

    GameObject plane = GameObject::createGameObject();
    plane.m_model = planeModel;
    plane.m_transform.translation = {0.f, 0.f, 2.f};
    plane.m_transform.rotation.z = glm::radians(45.f);
    m_gameObjects.push_back(std::move(plane));

    GameObject cube = GameObject::createGameObject();
    cube.m_model = cubeModel;
    cube.m_transform.translation = {0.f, 0.f, 2.5f};
    cube.m_transform.scale = {.5f, .5f, .5f};
    m_gameObjects.push_back(std::move(cube));
}

void CubeApp::run () {
    loadGameObjects();

    Device& device = m_engine.getDevice();
    Renderer& renderer = m_engine.getRenderer();
    Window& window = m_engine.getWindow();
    std::unique_ptr<DescriptorPool>& globalPool = m_engine.getGlobalPool();

    // UBOs
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for ( size_t i = 0; i < uboBuffers.size(); ++i ) {
        uboBuffers[ i ] = std::make_unique<Buffer>(
            device, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            device.properties.limits.minUniformBufferOffsetAlignment);

        uboBuffers[ i ]->map();
    }

    // Global descriptor set
    auto globalSetLayout = DescriptorSetLayout::Builder(device)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                               .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for ( size_t i = 0; i < globalDescriptorSets.size(); ++i ) {
        VkDescriptorBufferInfo info = uboBuffers[ i ]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &info)
            .build(globalDescriptorSets[ i ]);
    }

    // Texture, default white on cube
    m_textures.push_back(
        std::make_unique<Texture>(device, std::string(EXAMPLE_TEXTURE_DIR) + "/perryTheChad.jpg"));

    auto textureSetLayout =
        DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

    std::vector<VkDescriptorSet> textureDescriptorSets(m_textures.size());
    for ( size_t i = 0; i < m_textures.size(); ++i ) {
        VkDescriptorImageInfo info = m_textures[ i ]->descriptorInfo();
        DescriptorWriter(*textureSetLayout, *globalPool)
            .writeImage(0, &info)
            .build(textureDescriptorSets[ i ]);
    }

    m_defaultTexture = Texture::createDefaultWhite(device);

    VkDescriptorImageInfo defaultInfo = m_defaultTexture->descriptorInfo();
    VkDescriptorSet defaultTextureSet;
    DescriptorWriter(*textureSetLayout, *globalPool).writeImage(0, &defaultInfo).build(defaultTextureSet);

    m_gameObjects[ 0 ].m_textureDescriptorSet = textureDescriptorSets[ 0 ]; // plane = perry
    m_gameObjects[ 1 ].m_textureDescriptorSet = defaultTextureSet;          // cube = white

    RenderSystem renderSystem {
        device,
        renderer.getSwapChainRenderPass(),
        globalSetLayout->getDescriptorSetLayout(),
        textureSetLayout->getDescriptorSetLayout(),
    };

    Camera camera {};
    float aspect = renderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

    GameObject viewObject = GameObject::createGameObject();
    KeyboardController cameraController {};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while ( !window.shouldClose() ) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float deltaTime =
            std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        deltaTime = std::min(deltaTime, 0.1f);

        cameraController.moveInPlaneXZ(window.getGLFWWindow(), deltaTime, viewObject);
        cameraController.otherKeys(window.getGLFWWindow(), deltaTime, viewObject);
        camera.setViewXYZ(viewObject.m_transform.translation, viewObject.m_transform.rotation);

        if ( aspect != renderer.getAspectRatio() ) {
            aspect = renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
        }

        TransformComponent& cubeT = m_gameObjects[ 1 ].m_transform;
        cubeT.rotation.x = glm::mod(cubeT.rotation.x + 0.3f * deltaTime, glm::two_pi<float>());
        cubeT.rotation.y = glm::mod(cubeT.rotation.y + 0.5f * deltaTime, glm::two_pi<float>());

        if ( VkCommandBuffer cmd = renderer.beginFrame() ) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo {
                .frameIndex = frameIndex,
                .deltaTime = deltaTime,
                .commandBuffer = cmd,
                .camera = camera,
                .globalDescriptorSet = globalDescriptorSets[ frameIndex ],
            };

            GlobalUbo ubo {};
            ubo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers[ frameIndex ]->writeToBuffer(&ubo);

            renderer.beginSwapChainRenderPass(cmd);
            renderSystem.renderGameObject(frameInfo, m_gameObjects);
            renderer.endSwapChainRenderPass(cmd);

            renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(device.getDevice());
}