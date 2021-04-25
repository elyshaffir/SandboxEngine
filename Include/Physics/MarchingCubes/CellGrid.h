#pragma once

#include <array>
#include <memory>
#include <Physics/MarchingCubes/Cell.h>
#include <Physics/MarchingCubes/MarchingCubes.h>

namespace sandbox
{
	/*
	 * Represents a fixed size grid of cells.
	 * The Z axis is up.
	 */
	class CellGrid
	{
	public:
		CellGrid();

		std::array<std::array<std::array<std::unique_ptr<Cell>, MarchingCubes::GRID_SIZE>,
							  MarchingCubes::GRID_SIZE>, MarchingCubes::GRID_SIZE> cells;
	};
}
