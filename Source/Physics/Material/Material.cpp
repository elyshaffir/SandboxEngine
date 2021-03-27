#include <Physics/Material/Material.h>

sandbox::Material::Material(const std::string& name, const sandbox::MaterialStateTransitions & stateTransitions,
							sandbox::Physics::Density averageDensity) :
		name(name), stateTransitions(stateTransitions), averageDensity(averageDensity)
{
}
