#ifndef SANDBOXENGINE_CHUNK_H
#define SANDBOXENGINE_CHUNK_H

#include <memory>

#include <Physics/MarchingCubes/CellGrid.h>

namespace sandbox
{
	class Chunk
	{
	public:
		Chunk();

		/*
		 * Updates the Cells in the CellGrid according to physics.
		 */
		void Update();

	private:
		std::unique_ptr<CellGrid> cellGrid;
	};
}

#endif //SANDBOXENGINE_CHUNK_H
