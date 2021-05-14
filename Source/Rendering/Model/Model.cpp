#include <Rendering/Model/Model.h>

sandbox::Model::Model(std::vector<Vertex> & vertices) : vertexBuffer(vertices), vertexCount(vertices.size())
{
}

void sandbox::Model::Destroy(VkDevice device)
{
	vertexBuffer.Destroy(device);
}

void sandbox::Model::Bind(VkCommandBuffer commandBuffer) const
{
	VkBuffer buffers[] = {vertexBuffer.buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void sandbox::Model::Draw(VkCommandBuffer commandBuffer) const
{
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}
