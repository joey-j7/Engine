#include "pch.h"
#include "WorldManager.h"

#include "Engine/Application.h"

namespace Engine
{
	WorldManager* WorldManager::s_Instance = nullptr;

	WorldManager::WorldManager()
	{
		CB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}

	WorldManager::~WorldManager()
	{

	}
}
