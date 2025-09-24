#include "Pipeline.hpp"

namespace MAGE {

	std::vector<char> Pipeline::readFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios:ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("FAILED TO OPEN FILE: " + fileName);
		}
	}




}