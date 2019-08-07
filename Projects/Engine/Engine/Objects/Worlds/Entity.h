#pragma once

#include "Engine/Objects/RealTimeObject.h"

namespace Engine
{
	class World;

	class Engine_API Entity : public RealTimeObject
	{
		friend class World;

		Entity(const std::string& sName = "Unnamed Entity");
		World* GetWorld() const { return m_pWorld; }

	protected:
		virtual void Awake() {};

		virtual void Update(float fDeltaTime) override {};
		virtual void FixedUpdate(float fDeltaTime) override {};

		virtual void Draw(float fDeltaTime) override {};
		virtual void LateUpdate(float fDeltaTime) override {};

		World* m_pWorld = nullptr;
		uint32_t m_uiID = 0;
	};
}