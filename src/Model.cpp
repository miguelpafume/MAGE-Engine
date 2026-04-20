#include "Model.hpp"

namespace MAGE {

Model::Model(Device& device, const Builder& builder) : m_device(device) {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Model::~Model() {
    // Buffers are automatically cleaned up by their destructors
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    m_vertexCount = static_cast<size_t>(vertices.size());
    assert(m_vertexCount >= 3 && "VERTEX COUNT MUST BE AT LEAST 3");
    
    uint32_t vertexSize = sizeof(vertices[0]);  
    VkDeviceSize bufferSize = vertexSize * m_vertexCount;

    Buffer stagingBuffer (
        m_device,
        vertexSize,
        m_vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    m_vertexBuffer = std::make_unique<Buffer>(
        m_device,
        vertexSize,
        m_vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    m_device.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0;

    if (!m_hasIndexBuffer) return;
    
    uint32_t indexSize = sizeof(indices[0]);
    VkDeviceSize bufferSize = indexSize * m_indexCount;
    
    Buffer stagingBuffer (
        m_device,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    m_indexBuffer = std::make_unique<Buffer> (
        m_device,
        indexSize,
        m_indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT  
    );

    m_device.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {m_vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    // ### CAN USE VK_INDEX_TYPE_UINT16 FOR SMALLER MODELS
    if (m_hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void Model::draw(VkCommandBuffer commandBuffer) {
    if (m_hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
        return;
    }
    else {
        vkCmdDraw(commandBuffer, m_vertexCount, 1, 0 ,0);
    }
}

} // namespace MAGE