#include <Physics/Material/Materials.h>

sandbox::Material sandbox::Materials::Air()
{
	return sandbox::Material("Air", {58, 78.80}, 0.87);
}
