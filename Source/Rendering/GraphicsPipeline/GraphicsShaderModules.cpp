#include <Rendering/GraphicsPipeline/GraphicsShaderModules.h>
#include <IO/FileIO.h>

#include <stdexcept>

static void CreateShaderModule(VkDevice device, const std::vector<char> & code, VkShaderModule * module)
{
	VkShaderModuleCreateInfo createInfo = { };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, module) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
}

sandbox::GraphicsShaderModules::GraphicsShaderModules(VkDevice device, const GraphicsShaderPaths & shaderPaths)
		: vertexShaderModule(VK_NULL_HANDLE), fragmentShaderModule(VK_NULL_HANDLE)
{
	CreateShaderModule(device, FileIO::ReadFile(shaderPaths.vertexShaderPath), &vertexShaderModule);
	CreateShaderModule(device, FileIO::ReadFile(shaderPaths.fragmentShaderPath), &fragmentShaderModule);
}

void sandbox::GraphicsShaderModules::Destroy(VkDevice device)
{
	vkDestroyShaderModule(device, vertexShaderModule, nullptr);
	vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}
