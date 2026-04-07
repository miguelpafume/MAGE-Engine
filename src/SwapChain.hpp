#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "Device.hpp"

namespace MAGE {

class SwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 3;        

    SwapChain(Device &deviceRef, VkExtent2D windowExtent);
    SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previousSwapChain);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    SwapChain& operator=(const SwapChain &) = delete;

    VkFormat findDepthFormat();
    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    VkFramebuffer   getFrameBuffer(int index)   const { return m_swapChainFramebuffers[index]; }
    VkRenderPass    getRenderPass()             const { return m_renderPass; }
    VkImageView     getImageView(int index)     const { return m_swapChainImageViews[index]; }
    size_t          getImageCount()             const { return m_swapChainImages.size(); }
    VkFormat        getSwapChainImageFormat()   const { return m_swapChainImageFormat; }
    VkExtent2D      getSwapChainExtent()        const { return m_swapChainExtent; }
    uint32_t        getWidth()                  const { return m_swapChainExtent.width; }
    uint32_t        getHeight()                 const { return m_swapChainExtent.height; }
    float           extentAspectRatio()         const { return static_cast<float>(m_swapChainExtent.width) / static_cast<float>(m_swapChainExtent.height); }
    
    bool compareSwapChainFormats(const SwapChain& swapChain) const {
        return swapChain.m_swapChainDepthFormat == m_swapChainDepthFormat && swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
    }

private:
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();
    void init();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat m_swapChainImageFormat;
    VkFormat m_swapChainDepthFormat;
    VkExtent2D m_swapChainExtent;

    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkRenderPass m_renderPass;
    std::vector<VkImage> m_depthImages;
    std::vector<VkDeviceMemory> m_depthImageMemorys;
    std::vector<VkImageView> m_depthImageViews;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;

    Device &m_device;
    VkExtent2D m_windowExtent;
    VkSwapchainKHR m_swapChain;
    std::shared_ptr<SwapChain> m_oldSwapChain;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
    size_t m_currentFrame = 0;
};

} // namespace MAGE