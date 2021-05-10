#pragma once

#include <Rendering/Model/Vertex.h>

#include <vector>

namespace sandbox
{
	struct BufferAllocationData
	{
		void * data;
		VkDeviceSize size;
		VkDeviceMemory & memory;
	};

	class VertexBuffer
	{
	public:
		VkBuffer buffer;

		explicit VertexBuffer(std::vector<Vertex> & vertices);

		BufferAllocationData GenerateAllocationData();

		void Destroy(VkDevice device);

	private:
		VkDeviceMemory memory;
		Vertex * data;
		VkDeviceSize size;
	};
}
