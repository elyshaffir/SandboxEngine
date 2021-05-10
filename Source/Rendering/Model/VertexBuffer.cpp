#include <Rendering/Model/VertexBuffer.h>

#include <stdexcept>

sandbox::VertexBuffer::VertexBuffer(std::vector<Vertex> & vertices)
		: buffer(), memory(), data(vertices.data()), size(sizeof(Vertex) * static_cast<uint32_t>(vertices.size()))
{
	if (vertices.size() < 3)
	{
		throw std::runtime_error("Vertex count for model must be at least 3");
	}
}

sandbox::BufferAllocationData sandbox::VertexBuffer::GenerateAllocationData()
{
	return {data, size, memory};
}

void sandbox::VertexBuffer::Destroy(VkDevice device)
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, memory, nullptr);
}
