#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

class DebugWorld : public Engine::World
{
public:
	DebugWorld();

protected:
	virtual void Draw(float fDeltaTime) override;

private:
	Engine::DrawPass* m_pPass = nullptr;
};
