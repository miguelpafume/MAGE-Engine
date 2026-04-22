#pragma once

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Device.hpp"
#include "Buffer.hpp"

namespace MAGE {

class Texture {
public:
    // Load from file
    Texture(Device& device, const std::string& imagePath);

    // Create from raw pixel data
    Texture(Device& device, const uint8_t* pixels, uint32_t width, uint32_t height);
    ~Texture();

    Texture(const Texture &) = delete;
    Texture& operator=(const Texture &) = delete;
    
    VkDescriptorImageInfo descriptorInfo() const;

    static std::unique_ptr<Texture> createDefaultWhite(Device& device);

    void update(const uint8_t* pixels, uint32_t width, uint32_t height);

    uint32_t getWidth()     const { return m_width; }
    uint32_t getHeight()    const { return m_height; }

private:
    void createTextureImage(const uint8_t* pixels, uint32_t width, uint32_t height);
    void createImageView();
    void createSampler();
    
    Device& m_device;

    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_mipLevels;
};

} // namespace MAGE