#include "Buffer.hpp"

namespace MAGE {

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
	if (minOffsetAlignment > 0) {
		return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}

	return instanceSize;
}

Buffer::Buffer(Device &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
			: m_device{device}, m_instanceSize{instanceSize}, m_instanceCount{instanceCount}, m_usageFlags{usageFlags}, m_memoryPropertyFlags{memoryPropertyFlags} {
	m_alignmentSize = getAlignment(m_instanceSize, minOffsetAlignment);
	m_bufferSize = m_alignmentSize * m_instanceCount;
	m_device.createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_memory);
}

Buffer::~Buffer() {
	unmap();
	vkDestroyBuffer(m_device.getDevice(), m_buffer, nullptr);
	vkFreeMemory(m_device.getDevice(), m_memory, nullptr);
}

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
	assert(m_buffer && m_memory && "CALLED MAP ON BUFFER BEFORE CREATE!");

	return vkMapMemory(m_device.getDevice(), m_memory, offset, size, 0, &m_mapped);
}

void Buffer::unmap() {
	if (m_mapped) {
		vkUnmapMemory(m_device.getDevice(), m_memory);
		m_mapped = nullptr;
	}
}

void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
	assert(m_mapped && "CANNOT COPY TO UNMAPPED BUFFER!");

	if (size == VK_WHOLE_SIZE) {
		memcpy(m_mapped, data, m_bufferSize);
	}
	else {
		char *memOffset = (char *)m_mapped;
		memOffset += offset;
		memcpy(memOffset, data, size);
	}
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
	VkMappedMemoryRange mappedRange = {
		.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		.memory = m_memory,
		.offset = offset,
		.size = size
	};

	return vkFlushMappedMemoryRanges(m_device.getDevice(), 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
	VkMappedMemoryRange mappedRange = {
		.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		.memory = m_memory,
		.offset = offset,
		.size = size
	};
	
	return vkInvalidateMappedMemoryRanges(m_device.getDevice(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
	return VkDescriptorBufferInfo{m_buffer, offset, size};
}

void Buffer::writeToIndex(void *data, int index) {
	writeToBuffer(data, m_instanceSize, index * m_alignmentSize);
}

VkResult Buffer::flushIndex(int index) {
	return flush(m_alignmentSize, index * m_alignmentSize);
}

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
	return descriptorInfo(m_alignmentSize, index * m_alignmentSize);
}

VkResult Buffer::invalidateIndex(int index) {
	return invalidate(m_alignmentSize, index * m_alignmentSize);
}

}  // namespace MAGE