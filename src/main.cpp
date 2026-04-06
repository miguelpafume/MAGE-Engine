#include <iostream>

#include <vulkan/vulkan.h>

#include "Engine.hpp"

int main() {
	MAGE::Engine app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}