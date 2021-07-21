#pragma once

#include "Engine/Core.h"
#include <string>

// The most simple class is called an object, an object has a name and can exist outside of any world

// A dynamic object is an object that is able to update itself, and can dynamically
// provide itself each frame through for ex. Update() and Draw()
// A layered object is a dynamic object containing a stack of layers, each layer being updated through the stack
// A layer is a dynamic object that can be attached to a layer stack, it can be paused and stopped

// World objects are called entities
// An entity is an abstract class, which can contain components. Entities exist in
// worlds and are mostly "physically" visible, whereas object are always invisible
// A static entity is an object+entity that is not able to update itself and has to provide
// itself through events, keeps a minimal footprint
// A dynamic entity is an object+entity that is able to update itself, and can dynamically
// provide itself each frame through for ex. Update() and Draw()

namespace Engine
{
	class Engine_API Object
	{
	public:
		Object(const std::string& sName = "Object");

		void SetName(const std::string& sName) { m_sName = sName; }
		const std::string& GetName() const { return m_sName; }

	protected:
		std::string m_sName = "";
	};
}
