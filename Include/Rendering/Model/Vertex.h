#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace sandbox
{
	struct Vertex
	{
		glm::vec2 position;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};
}
