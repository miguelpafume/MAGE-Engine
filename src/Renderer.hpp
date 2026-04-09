#pragma once

#include <memory>
#include <vector>
#include <cassert>

#include "Window.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"

namespace MAGE {

class Renderer {
public:
	Renderer(Window& window, Device& device);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    VkRenderPass    getSwapChainRenderPass()    const { return m_swapChain->getRenderPass(); }
    bool            getIsFrameInProgress()      const { return m_isFrameStarted; }
    float           getAspectRatio()            const { return m_swapChain->extentAspectRatio(); }
    
    VkCommandBuffer  getcurrentCommandBuffer() const {
        assert(m_isFrameStarted && "CANNOT GET COMMAND BUFFER IF FRAME NOT IN PROGRESS");
        return m_commandBuffers[m_currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(m_isFrameStarted && "CANNOT GET FRAME INDEX IF FRAME NOT IN PROGRESS");
        return m_currentFrameIndex;
    }

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	void createCommandBuffers();
	void recreateSwapChain();
	void freeCommandBuffers();

	Window& m_window;
	Device& m_device;
	std::unique_ptr<SwapChain> m_swapChain;
	
	std::vector<VkCommandBuffer> m_commandBuffers;

    uint32_t m_currentImageIndex;
    int m_currentFrameIndex = 0;
    bool m_isFrameStarted = false;
};

} // namespace MAGE