#include "pch.h"
#include "DebugWorld.h"

#include "Engine/Application.h"
#include "Rendering/CommandEngine.h"

DebugWorld::DebugWorld()
{
	// Engine::CommandEngine* pEngine = Engine::Application::Get().GetRenderContext().GetAPI().GetCommandEngine("Screen");

	// Engine::ShaderProgram::Descriptor shaderDesc;
	// shaderDesc.Vertex = "default.vert";
	// shaderDesc.Pixel = "default.frag";

	// m_pPass = pEngine->Create(shaderDesc);
}

void DebugWorld::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);

	// m_pPass->Begin();
	// m_pPass->Clear();
	// m_pPass->Draw();
	// m_pPass->End();
}
