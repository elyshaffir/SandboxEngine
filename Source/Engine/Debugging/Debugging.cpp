#include <Engine/Debugging/Debugging.h>

static void InitializeGoogleLogging(char * argv0)
{
#ifdef LOG_TO_STDERR
	FLAGS_logtostderr = true;
#else
	FLAGS_log_dir = "D:/DATA/Documents/Vulkan/SandboxEngine";
#endif
	google::InitGoogleLogging(argv0);
}

void sandbox::Debugging::Initialize(char * argv0)
{
	InitializeGoogleLogging(argv0);
}