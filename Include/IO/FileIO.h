#ifndef SANDBOXENGINE_FILEIO_H
#define SANDBOXENGINE_FILEIO_H

#include <string>
#include <vector>

namespace sandbox
{
	class FileIO
	{
	public:
		static std::vector<char> ReadFile(const std::string & path);
	};
}

#endif //SANDBOXENGINE_FILEIO_H
