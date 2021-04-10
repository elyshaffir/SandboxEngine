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

	PipelineConfigurationInfo pipelineConfigurationInfo({800, 600});

	sandbox::Renderer renderer(800, 600, "Good job, me!", defaultShaders, pipelineConfigurationInfo);

	while (!renderer.ShouldClose())
	{
		LOG(INFO) << "Application update!";
		Engine::Update();
		renderer.DrawFrame();
	}
	renderer.WaitIdle();
	Engine::TearDown();
}
