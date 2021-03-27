#ifndef SANDBOXENGINE_MATERIALSTATE_H
#define SANDBOXENGINE_MATERIALSTATE_H

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

#endif //SANDBOXENGINE_MATERIALSTATE_H
