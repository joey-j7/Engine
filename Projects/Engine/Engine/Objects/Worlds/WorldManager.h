#pragma once

#include "Engine/Objects/Layers/Layer.h"
#include "Engine/Objects/Layers/Layerable.h"
#include "Engine/Objects/Worlds/World.h"

// WorldManager is an application layer manager of worlds

namespace Engine
{
	class Engine_API WorldManager : public Object, public Layer, public Layerable<World>
	{
	public:
		WorldManager();
		~WorldManager();

		static WorldManager& Get() { return *s_Instance; }

	private:
		static WorldManager* s_Instance;
	};
}
