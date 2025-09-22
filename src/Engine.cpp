#include "Engine.hpp"
namespace MAGE {

void Engine::run() {
	while (!m_window.shouldClose()) {
		glfwPollEvents();
	}
}

} // namespace MAGE