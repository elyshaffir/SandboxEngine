#pragma once

#include <string>
#include <Physics/Material/MaterialState.h>

namespace sandbox
{
	class Material
	{
	public:
		Material(const std::string& name, const MaterialStateTransitions & stateTransitions, Physics::Density averageDensity);

	private:
		const std::string name;
		const MaterialStateTransitions stateTransitions;
		const Physics::Density averageDensity;
	};
}
