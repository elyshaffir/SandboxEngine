#include <IO/FileIO.h>
#include <fstream>
#include <IO/FileOpenException.h>

std::vector<char> sandbox::FileIO::ReadFile(const std::string & path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw FileOpenException(path);
	}
	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> fileBuffer(fileSize);

	file.seekg(0);
	file.read(fileBuffer.data(), fileSize);
	file.close();

	return fileBuffer;
}