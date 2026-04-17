#include "Model.hpp"

namespace MAGE {

Model::Model(Device& device, const Builder& builder) : m_device(device) {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Model::~Model() {
    vkDestroyBuffer(m_device.getDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(m_device.getDevice(), m_vertexBufferMemory, nullptr);

    if (m_hasIndexBuffer) {
        vkDestroyBuffer(m_device.getDevice(), m_indexBuffer, nullptr);
        vkFreeMemory(m_device.getDevice(), m_indexBufferMemory, nullptr);
    }
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    m_vertexCount = static_cast<size_t>(vertices.size());
    assert(m_vertexCount >= 3 && "VERTEX COUNT MUST BE AT LEAST 3");
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
    
    m_device.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_vertexBuffer,
        m_vertexBufferMemory
    );
    
    void* data;
    vkMapMemory(m_device.getDevice(), m_vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_device.getDevice(), m_vertexBufferMemory);
}

void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndexBuffer = m_indexCount > 0;

    if (!m_hasIndexBuffer) return;
    
    VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
    
    m_device.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_indexBuffer,
        m_indexBufferMemory
    );
    
    void* data;
    vkMapMemory(m_device.getDevice(), m_indexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(m_device.getDevice(), m_indexBufferMemory);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    // ### CAN USE VK_INDEX_TYPE_UINT16 FOR SMALLER MODELS
    if (m_hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
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