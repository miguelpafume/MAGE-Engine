#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <unordered_map>

#include "Device.hpp"

namespace MAGE {

class DescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(Device &m_device) : m_device{m_device} {}

        Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> build() const;

    private:
        Device &m_device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings {};
    };

    DescriptorSetLayout(Device &m_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

private:
    Device &m_device;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

    friend class DescriptorWriter;
};

class DescriptorPool {
public:
    class Builder {
    public:
        Builder(Device &m_device) : m_device{m_device} {}

        Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPool> build() const;

    private:
        Device &m_device;
        std::vector<VkDescriptorPoolSize> m_poolSizes {};
        uint32_t m_maxSets = 1000;
        VkDescriptorPoolCreateFlags m_poolFlags = 0;
    };

    DescriptorPool(Device &m_device, uint32_t m_maxSets, VkDescriptorPoolCreateFlags m_poolFlags, const std::vector<VkDescriptorPoolSize> &m_poolSizes);
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool &) = delete;
    DescriptorPool &operator=(const DescriptorPool &) = delete;

    bool allocateDescriptor(const VkDescriptorSetLayout m_descriptorSetLayout, VkDescriptorSet &descriptor) const;
    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
    void resetPool();

private:
    Device &m_device;
    VkDescriptorPool m_descriptorPool;

    friend class DescriptorWriter;
};

class DescriptorWriter {
public:
    DescriptorWriter(DescriptorSetLayout &m_setLayout, DescriptorPool &m_pool);

    DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

private:
    DescriptorSetLayout &m_setLayout;
    DescriptorPool &m_pool;
    std::vector<VkWriteDescriptorSet> m_writes;
};

}  // namespace MAGE