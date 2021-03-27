#ifndef SANDBOXENGINE_FILEOPENEXCEPTION_H
#define SANDBOXENGINE_FILEOPENEXCEPTION_H

#include <stdexcept>

namespace sandbox
{
	class FileOpenException : public std::runtime_error
	{
	public:
		explicit FileOpenException(const std::string & badFilePath);
	};
}

#endif //SANDBOXENGINE_FILEOPENEXCEPTION_H
