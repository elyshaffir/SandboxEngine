#ifndef SANDBOXENGINE_GRAPHICSSHADERMODULES_H
#define SANDBOXENGINE_GRAPHICSSHADERMODULES_H

#include <string>
#include <array>

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

		std::array<VkPipelineShaderStageCreateInfo, 2> GetCreateInfos() const;

		void Destroy(VkDevice device) const;

	private:
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};
}

#endif //SANDBOXENGINE_GRAPHICSSHADERMODULES_H
