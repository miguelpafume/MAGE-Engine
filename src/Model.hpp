#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <cassert>
#include <cstring>
#include <memory>

#include "Util.hpp"
#include "Device.hpp"
#include "Buffer.hpp"

namespace MAGE {

class Model {
public:   
    Model(Device& device, const Builder& builder);
	~Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);
    void createIndexBuffers(const std::vector<uint32_t>& indices);

    Device &m_device;

    std::unique_ptr<Buffer> m_vertexBuffer;
    uint32_t m_vertexCount;

    bool m_hasIndexBuffer = false;
    std::unique_ptr<Buffer> m_indexBuffer;
    uint32_t m_indexCount;
};

} // namespace MAGE