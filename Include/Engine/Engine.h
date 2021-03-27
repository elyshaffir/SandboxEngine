#ifndef SANDBOXENGINE_ENGINE_H
#define SANDBOXENGINE_ENGINE_H

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

#endif //SANDBOXENGINE_ENGINE_H
