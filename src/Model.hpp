#pragma once

#include "Device.hpp"

namespace MAGE {

class Model {
public:
	Model();
	~Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    Device &m_device;

    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    uint32_t m_vertexCount;
};

} // namespace MAGE