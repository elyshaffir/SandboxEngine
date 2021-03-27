#ifndef SANDBOXENGINE_GRAPHICSSHADERMODULES_H
#define SANDBOXENGINE_GRAPHICSSHADERMODULES_H

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace sandbox
{
	struct GraphicsShaderPaths
	{
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};

	class GraphicsShaderModules
	{
	public:
		GraphicsShaderModules(VkDevice device, const GraphicsShaderPaths & shaderPaths);

		void Destroy(VkDevice device);

	private:
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};
}

#endif //SANDBOXENGINE_GRAPHICSSHADERMODULES_H
