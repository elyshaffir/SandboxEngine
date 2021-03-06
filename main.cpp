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

	std::vector<Vertex> vertices = {
			{{0.0f,  -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
	};
	auto model = Model(vertices);

	sandbox::Renderer renderer(windowConfigurationInfo, defaultShaders, model);

	while (!renderer.ShouldClose())
	{
		Engine::Update();
		renderer.DrawFrame();
	}
	renderer.WaitIdle();
	Engine::TearDown();
}
