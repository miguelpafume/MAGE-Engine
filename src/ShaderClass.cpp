#include "ShaderClass.hpp"

std::vector<char> readFile(const char* filename) {
	std::ifstream file(filename, std::ios::binary);

	if (!file) {
		throw std::runtime_error("FAILED TO OPEN FILE!");
	}

	std::vector<char> content;

	file.seekg(0, std::ios::end);
	content.resize(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(&content[0], content.size());
	
	file.close();

	return content;
}

