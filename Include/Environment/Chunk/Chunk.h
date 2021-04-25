#pragma once

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
