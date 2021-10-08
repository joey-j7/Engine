#pragma once

#include "Engine/Core.h"
#include "Engine/General/Common.h"

#include <stdexcept>

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
		Object(const String& sName = "Object");
		virtual ~Object();

		void SetName(const String& sName) { m_sName = sName; }
		virtual const String& GetName() const { return m_sName; }

		template <class T>
		static size_t GetClassID()
		{
			static_assert(
				std::is_base_of<Object, T>::value,
				"Type is not an Object"
			);

			return typeid(T).hash_code();
		}

		// Disable new keyword
		// inline void* operator new (std::size_t) throw(std::bad_alloc) {
		// 	extern void* bare_new_erroneously_called();
		// 	return bare_new_erroneously_called();
		// }
		
	protected:
		String m_sName = "";
	};
}
