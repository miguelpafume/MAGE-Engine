#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace MAGE {

class ImGUIRenderSystem {
public:
    ImGUIRenderSystem();
    ~ImGUIRenderSystem();

    VkCommandBuffer prepareCommandBuffer(int image);

	ImGUIRenderSystem(const ImGUIRenderSystem&) = delete;
	ImGUIRenderSystem& operator=(const ImGUIRenderSystem&) = delete;

private:
    void CreateDescriptorPool();

    

};

} // namespace MAGE