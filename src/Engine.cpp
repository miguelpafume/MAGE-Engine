#include "Engine.hpp"

#include "SwapChain.hpp"

namespace MAGE {

Engine::Engine(std::string windowTitle) : m_windowTitle{windowTitle} { 
	m_globalPool = DescriptorPool::Builder(m_device)
		.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT + 10)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT) // UBOs
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10) // Texture samplers (max 10)
		.build();
}

Engine::~Engine() {}

} // namespace MAGE