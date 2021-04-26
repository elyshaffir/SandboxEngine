#pragma once

#include <Rendering/Queue/Queue.h>

namespace sandbox
{
	class PresentQueue : public Queue
	{
	public:
		PresentQueue(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	private:
		void FindFamily(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	};
}
