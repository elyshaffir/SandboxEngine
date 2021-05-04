#pragma once

#include <Rendering/Queue/Queue.h>

namespace sandbox
{
	class PresentQueue : public Queue
	{
	public:
		void Create(VkDevice device);

		bool FindFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	};
}
