#include <Engine/Debugging/Debugging.h>

static void InitializeGoogleLogging(char * argv0)
{
	FLAGS_logtostderr = true;
	google::InitGoogleLogging(argv0);
}

void sandbox::Debugging::Initialize(char * argv0)
{
	InitializeGoogleLogging(argv0);
}