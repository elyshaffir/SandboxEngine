#pragma once

#include <Rendering/Model/VertexBuffer.h>

namespace sandbox
{
	class Model
	{
	public:
		VertexBuffer vertexBuffer;

		explicit Model(std::vector<Vertex> & vertices);

		void Destroy(VkDevice device);

		void Bind(VkCommandBuffer commandBuffer) const;

		void Draw(VkCommandBuffer commandBuffer) const;

	private:
		uint32_t vertexCount;
	};
}
