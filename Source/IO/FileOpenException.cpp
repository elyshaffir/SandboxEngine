#include <IO/FileOpenException.h>

sandbox::FileOpenException::FileOpenException(const std::string & badFilePath) :
		runtime_error("Couldn't open file at " + badFilePath)
{
}
