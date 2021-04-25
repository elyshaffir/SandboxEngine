#pragma once

#include <vector>

namespace sandbox
{
	class Engine
	{
	public:
		static void Initialize(char * argv0);

		static void Update();

		static void TearDown();

		static std::vector<const char *> GetRequiredGLFWExtensions();
	};
}