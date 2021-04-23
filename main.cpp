#include <iostream>

#include <glog/logging.h>
#include <Rendering/Window/Window.h>
#include <Rendering/GraphicsPipeline/GraphicsPipeline.h>
#include <Engine/Engine.h>
#include <Rendering/Renderer.h>

using namespace sandbox;

int main(int argc, char * argv[])
{
	Engine::Initialize(argv[0]);

	GraphicsShaderPaths defaultShaders {
			"compiled-shaders/default_vertex.spv",
			"compiled-shaders/default_fragment.spv"
	};

	WindowConfigurationInfo windowConfigurationInfo = {800, 500, "Good job, me!"};
	PipelineConfigurationInfo pipelineConfigurationInfo({800, 500});

	sandbox::Renderer renderer(windowConfigurationInfo, defaultShaders, pipelineConfigurationInfo);

	while (!renderer.ShouldClose())
	{
		LOG(INFO) << "Application update!";
		Engine::Update();
		renderer.DrawFrame();
	}
	renderer.WaitIdle();
	Engine::TearDown();
}
