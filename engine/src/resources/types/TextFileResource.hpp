#ifndef TEXT_FILE_RESOURCE_HPP
#define TEXT_FILE_RESOURCE_HPP

#include "core/pch.hpp"
#include "resources/Resource.hpp"

namespace TileBite {

class TextFileResource : public Resource<TextFileResource> {
	SETUP_ID(Resource, TextFileResource)
public:
	TextFileResource(const std::string& resourceName, const std::string& filePath);

	std::string& getData() { return m_fileContent; }
	virtual bool isValid() override;

	TextFileResource(TextFileResource&&) noexcept = default;
	TextFileResource& operator=(TextFileResource&&) noexcept = default;
	TextFileResource(const TextFileResource&) = delete;
	TextFileResource& operator=(const TextFileResource&) = delete;
private:
	virtual bool createImplementation() override;
	virtual bool destroyImplementation() override;

	std::string m_filePath;
	std::string m_fileContent;
};

} // TileBite

#endif // !TEXT_FILE_RESOURCE_HPP
