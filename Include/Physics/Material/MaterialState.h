#pragma once

#include <Physics/Physics.h>

namespace sandbox
{
	enum class MaterialState
	{
		SOLID,
		LIQUID,
		GAS
	};

	struct MaterialStateTransitions
	{
		Physics::Temperature freezingTemperature;
		Physics::Temperature condensingTemperature;
	};
}
