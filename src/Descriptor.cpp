#include "Descriptor.hpp"

namespace MAGE
{

// Descriptor Set Layout Builder

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count) {
    assert(m_bindings.count(binding) == 0 && "BINDING ALREADY IN USE");

    VkDescriptorSetLayoutBinding layoutBinding{
        .binding = binding,
        .descriptorType = descriptorType,
        .descriptorCount = count,
        .stageFlags = stageFlags
    };

    m_bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
    return std::make_unique<DescriptorSetLayout>(m_device, m_bindings);
}

// Descriptor Set Layout

DescriptorSetLayout::DescriptorSetLayout(Device& m_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings) : m_device{m_device}, m_bindings{m_bindings} {
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings {};

    for (auto kv : m_bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(setLayoutBindings.size()),
        .pBindings = setLayoutBindings.data()
    };

    if (vkCreateDescriptorSetLayout(m_device.getDevice(),& descriptorSetLayoutInfo, nullptr,& m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE A DESCRIPTOR SET LAYOUT!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(m_device.getDevice(), m_descriptorSetLayout, nullptr);
}

// Descriptor Pool Builder

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count) {
    m_poolSizes.push_back({descriptorType, count});
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {
    m_poolFlags = flags;
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
    m_maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(m_device, m_maxSets, m_poolFlags, m_poolSizes);
}

// Descriptor Pool

DescriptorPool::DescriptorPool(Device& m_device, uint32_t m_maxSets, VkDescriptorPoolCreateFlags m_poolFlags, const std::vector<VkDescriptorPoolSize>& m_poolSizes)
                            : m_device{m_device} {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = m_poolFlags,
        .maxSets = m_maxSets,
        .poolSizeCount = static_cast<uint32_t>(m_poolSizes.size()),
        .pPoolSizes = m_poolSizes.data()
    };

    if (vkCreateDescriptorPool(m_device.getDevice(),& descriptorPoolInfo, nullptr,& m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE A DESCRIPTOR POOL!");
    }
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(m_device.getDevice(), m_descriptorPool, nullptr);
}

bool DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout m_descriptorSetLayout, VkDescriptorSet& descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = m_descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &m_descriptorSetLayout
    };

    if (vkAllocateDescriptorSets(m_device.getDevice(),& allocInfo,& descriptor) != VK_SUCCESS) {
        return false;
    }

    return true;
}

void DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(m_device.getDevice(), m_descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void DescriptorPool::resetPool() {
    vkResetDescriptorPool(m_device.getDevice(), m_descriptorPool, 0);
}

// Descriptor Writer

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& m_setLayout, DescriptorPool& m_pool) : m_setLayout{m_setLayout}, m_pool{m_pool} {}

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
    assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = m_setLayout.m_bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = bindingDescription.descriptorType,
        .pBufferInfo = bufferInfo
    };

    m_writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo) {
    assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = m_setLayout.m_bindings[binding];
    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = bindingDescription.descriptorType,
        .pImageInfo = imageInfo
    };

    m_writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(VkDescriptorSet& set) {
    bool success = m_pool.allocateDescriptor(m_setLayout.getDescriptorSetLayout(), set);
    if (!success) return false;

    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet& set) {
    for (auto& write : m_writes) {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(m_pool.m_device.getDevice(), m_writes.size(), m_writes.data(), 0, nullptr);
}

} // namespace MAGE