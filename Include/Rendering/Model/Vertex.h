#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace sandbox
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};
}
