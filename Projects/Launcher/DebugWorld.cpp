#include "pch.h"
#include "DebugWorld.h"

#include "Engine/Application.h"
#include "Rendering/CommandEngine.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform3DComponent.h"

#include "Engine/UI/Elements/UIComponent.h"

DebugWorld::DebugWorld()
{
	// Engine::CommandEngine* pEngine = Engine::Application::Get().GetRenderContext().GetAPI().GetCommandEngine("Screen");
	// 
	// Engine::ShaderProgram::Descriptor shaderDesc;
	// shaderDesc.Vertex = "default.vert";
	// shaderDesc.Pixel = "default.frag";
	// 
	// m_pPass = pEngine->Create(shaderDesc);
	//
	// Engine::Application::Get().GetHardwareContext().GetCamera().Start();

	Engine::StaticEntity* Entity = new Engine::StaticEntity();
	Entity->AddComponent<Engine::Transform2DComponent>();
	Entity->AddComponent<Engine::Transform3DComponent>();
	Entity->AddComponent<Engine::Transform2DComponent>();
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
