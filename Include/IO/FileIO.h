#pragma once

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
