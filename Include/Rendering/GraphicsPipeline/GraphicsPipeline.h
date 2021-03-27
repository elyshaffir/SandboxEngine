#ifndef SANDBOXENGINE_GRAPHICSPIPELINE_H
#define SANDBOXENGINE_GRAPHICSPIPELINE_H

#include <Rendering/Device/Device.h>
#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <Rendering/GraphicsPipeline/PipelineConfigurationInfo.h>

namespace sandbox
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(VkDevice device, const GraphicsShaderPaths & shaderPaths,
						 const PipelineConfigurationInfo & configurationInfo);

		void Create(VkDevice device, const PipelineConfigurationInfo & configurationInfo);

		void Destroy(VkDevice device);

	private:
		VkPipeline pipeline;
		GraphicsShaderModules shaderModules;
	};
}

#endif //SANDBOXENGINE_GRAPHICSPIPELINE_H
