#ifndef TEXT_FILE_RESOURCE_HPP
#define TEXT_FILE_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

namespace Engine {

class TextFileResource : public Resource<TextFileResource> {
	SETUP_ID(Resource, TextFileResource)
public:
	TextFileResource();
	TextFileResource(const std::string& resourceName, const std::string& filePath);
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	std::string m_filePath;
	std::string m_fileContent;
};

} // Engine

#endif // !TEXT_FILE_RESOURCE_HPP
