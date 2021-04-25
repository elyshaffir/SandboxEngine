#pragma once

#include <Physics/Material/Material.h>
#include <Physics/MarchingCubes/MarchingCubes.h>

namespace sandbox
{
	/*
	 * Represents a single cell in the marching cubes grid.
	 */
	class Cell
	{
	public:
		explicit Cell(const Material & currentMaterial);

	private:
		const Material & currentMaterial;
		MarchingCubes::SurfaceLevel surfaceLevel;
	};
}
