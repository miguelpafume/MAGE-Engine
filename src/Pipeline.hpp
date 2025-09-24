#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace MAGE {

class Pipeline {
public:
	Pipeline(const std::string& vertexFile, const std::string& fragmentFile);

private:
	static std::vector<char> readFile(const std::string& fileName);

	void createGraphicsPipeline(const std::string& vertexFile, const std::string& fragmentFile);
};

} // namespace MAGE