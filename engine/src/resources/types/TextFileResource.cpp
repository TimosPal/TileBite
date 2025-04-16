#include "resources/types/TextFileResource.hpp"

#include <fstream>
#include <sstream>

namespace Engine {

TextFileResource::TextFileResource(const std::string& resourceName, const std::string& filePath)
	: 
    Resource(resourceName),
    m_filePath(m_filePath), m_fileContent("")
{}

TextFileResource::TextFileResource()
	: TextFileResource("NoName", "")
{}

bool TextFileResource::createImplementation()
{
    // Open the file in binary mode to ensure no data is lost due to newline conversion
    std::ifstream file(m_filePath, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_fileContent = buffer.str();

    return true;
}

bool TextFileResource::destroyImplementation()
{
	m_fileContent.clear();
    return true;
}

} // Engine