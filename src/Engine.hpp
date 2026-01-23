#pragma once

#include "Window.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"

namespace MAGE {

class Engine {
public:
	void run();

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
	Device m_device{m_window};
	Pipeline m_pipeline{
		m_device,
		Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT),
		"simple_shader.vert.spv",
		"simple_shader.frag.spv"
	};
};

} // namespace MAGE