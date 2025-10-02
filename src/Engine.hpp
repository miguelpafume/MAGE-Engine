#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"

namespace MAGE {

class Engine {
public:
	void run();

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
	Pipeline m_pipeline{"simple_shader.vert.spv", "simple_shader.frag.spv" };
};

} // namespace MAGE