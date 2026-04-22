#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.hpp"

namespace MAGE {

Texture::Texture(Device& device, const std::string& imagePath) : m_device(device) {
    // Load image
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load(imagePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    
    if(!pixels) {
        throw std::runtime_error("FAILED TO LOAD TEXTURE IMAGE " + imagePath + "!");
    }
    
    createTextureImage(pixels, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    stbi_image_free(pixels);

    createImageView();
    createSampler();
}

Texture::Texture(Device& device, const uint8_t* pixels, uint32_t width, uint32_t height) : m_device(device) {
    createTextureImage(pixels, width, height);
    createImageView();
    createSampler();
}

Texture::~Texture() {
    vkDestroySampler(m_device.getDevice(), m_sampler, nullptr);
    vkDestroyImageView(m_device.getDevice(), m_imageView, nullptr);
    vkDestroyImage(m_device.getDevice(), m_image, nullptr);
    
    vkFreeMemory(m_device.getDevice(), m_imageMemory, nullptr);
}

VkDescriptorImageInfo Texture::descriptorInfo() const {
    return VkDescriptorImageInfo {
        .sampler = m_sampler,
        .imageView = m_imageView,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
}

void Texture::update(const uint8_t* pixels, uint32_t width = 0, uint32_t height = 0) {
    if (width == 0) width = m_width;
    if (height == 0) height = m_height;

    VkDeviceSize imageSize = width * height * 4;

    Buffer stagingBuffer {
        m_device,
        imageSize,
        1,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)pixels);

    m_device.transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    m_device.copyBufferToImage(stagingBuffer.getBuffer(), m_image, m_width, m_height, 1);
    m_device.transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels);
}

std::unique_ptr<Texture> Texture::createDefaultWhite(Device& device) {
    uint8_t pixel[] = {255, 255, 255, 255};
    return std::make_unique<Texture>(device, pixel, 1, 1);
}

void Texture::createTextureImage(const uint8_t* pixels, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_mipLevels = 1;
    
    VkDeviceSize imageSize = m_width * m_height * 4;

    // Staging buffer
    Buffer stagingBuffer {
        m_device,
        imageSize,
        1,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)pixels);


    // Create VkImage
    VkImageCreateInfo imageInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .extent = {m_width, m_height, 1},
        .mipLevels = m_mipLevels,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    m_device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_imageMemory);

    // Transition image layout and copy data
    m_device.transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    m_device.copyBufferToImage(stagingBuffer.getBuffer(), m_image, m_width, m_height, 1);
    m_device.transitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_mipLevels);
}

void Texture::createImageView() {
    VkImageViewCreateInfo viewInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = m_image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = m_mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    if (vkCreateImageView(m_device.getDevice(), &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE TEXTURE IMAGE VIEW!");
    }
}

void Texture::createSampler() {
    VkSamplerCreateInfo samplerInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR, // ###TODO Change both to NEAREST for a pixelated look
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = m_device.properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = static_cast<float>(m_mipLevels),
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    if (vkCreateSampler(m_device.getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER!");
    }    
}

} // namespace MAGE