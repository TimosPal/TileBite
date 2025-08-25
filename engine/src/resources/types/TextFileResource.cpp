#include "resources/types/TextFileResource.hpp"

namespace TileBite {

TextFileResource::TextFileResource(const std::string& resourceName, const std::string& filePath)
	: 
    Resource(resourceName, false),
    m_filePath(filePath), m_fileContent("")
{}

bool TextFileResource::createImplementation()
{
   // Open the file in binary mode to ensure no data is lost due to newline conversion
   std::ifstream file(m_filePath, std::ios::in | std::ios::binary);

   if (!file.is_open())
   {
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

bool TextFileResource::isValid()
{
	bool exists = std::filesystem::exists(m_filePath);
	if (!exists) LOG_ERROR("Invalid text file path {}", m_filePath);
	return exists;
}

} // TileBite