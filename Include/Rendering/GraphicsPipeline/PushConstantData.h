#pragma once

#include <glm/glm.hpp>

namespace sandbox
{
	struct PushConstantData
	{
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};
}
