#include "Window.hpp"

namespace MAGE {

class Engine {
public:
	void run();

private:
	static constexpr uint32_t WIDTH = 800;
	static constexpr uint32_t HEIGHT = 800;

	Window m_window{WIDTH, HEIGHT, "M.A.G.E."};
};

} // namespace MAGE