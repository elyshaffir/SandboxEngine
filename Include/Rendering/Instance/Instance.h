#pragma once

#include <vulkan/vulkan.h>
#include <array>

namespace sandbox
{
	class Instance
	{
	public:
#ifdef ENABLE_VALIDATION_LAYERS
		static constexpr std::array<const char *, 1> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
#endif

		VkInstance instance;

		Instance();

		~Instance();

	private:
#ifdef ENABLE_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT debugMessenger;

		void CreateDebugMessenger();

		void DestroyDebugMessenger();
#endif

		void Create();
	};
}
