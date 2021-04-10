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

		void Bind(VkCommandBuffer commandBuffer) const;

		void Destroy(VkDevice device);

	private:
		VkPipeline pipeline;
		GraphicsShaderModules shaderModules;
		VkPipelineLayout pipelineLayout;

		void Create(VkDevice device, const PipelineConfigurationInfo & configurationInfo);

		void CreateLayout(VkDevice device);
	};
}

#endif //SANDBOXENGINE_GRAPHICSPIPELINE_H
