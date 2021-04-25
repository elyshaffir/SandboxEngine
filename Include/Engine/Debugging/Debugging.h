#pragma once

#include <glog/logging.h>

namespace sandbox
{
	/*
	 * Handles debugging that can be generalized for an entire application.
	 */
	class Debugging
	{
	public:
		static void Initialize(char * argv0);
	};
}
