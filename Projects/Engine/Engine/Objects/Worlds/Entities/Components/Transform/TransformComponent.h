#pragma once

#include "Engine/Objects/Object.h"

#include "Engine/Objects/Worlds/Entities/Entity.h"
#include "Engine/Objects/Worlds/Entities/Components/Component.h"

namespace Engine
{
	enum TransformType
	{
		E_TRANSLATION = 0,
		E_ROTATION = 1,
		E_SCALING = 2
	};
	
	template<class T = Vector3>
	class Engine_API TransformComponent : public Component
	{
	public:
		const T& GetPosition(bool Local = true) const
		{
			return m_Position;
		}
		
		void SetPosition(const T& ToPosition, bool Local = true)
		{
			if (ToPosition == m_Position)
				return;

			MarkDirty(E_TRANSLATION);

			if (Local)
			{
				m_Position = ToPosition;
				return;
			}

			// TODO: Global
			m_Position = ToPosition;
		}
		
		void Translate(const T& Translation)
		{
			if (Translation == T(0.f))
				return;

			m_Position += Translation;
			MarkDirty();
		}
		
		const T& GetScale(bool Local = true) const
		{
			if (Local)
			{
				return m_Scale;
			}

			// TODO: Global
			return m_Scale;
		}
		
		void SetScale(const T& ToScale, bool Local = true)
		{
			if (ToScale == m_Scale)
				return;

			MarkDirty();

			if (Local)
			{
				m_Scale = ToScale;
				return;
			}

			// TODO: Global
			m_Scale = ToScale;
		}
		
		void Scale(const T& Scaling)
		{
			if (m_Scale == T(1.f))
				return;

			m_Scale *= Scaling;
			MarkDirty();
		}
		
		void MarkDirty(TransformType Type)
		{
			switch (Type)
			{
			case E_TRANSLATION:
				m_IsTranslationDirty = true;
				break;
			case E_ROTATION:
				m_IsRotationDirty = true;
				break;
			case E_SCALING:
			default:
				m_IsScaleDirty = true;
				break;
			}
		}

		bool IsDirty(TransformType Type) const
		{
			switch (Type)
			{
			case E_TRANSLATION:
				return m_IsTranslationDirty;
			case E_ROTATION:
				return m_IsRotationDirty;
			case E_SCALING:
			default:
				return m_IsScaleDirty;
			}
		}
		
	protected:
		TransformComponent(Entity& Entity, const std::string& sName = "Transform Component") : Component(Entity, sName)
		{
			m_ForceUniqueness = true;

			GetEntity().OnParentChange.Bind(
				this,
				&TransformComponent<T>::OnParentChange
			);
		}
		
		virtual ~TransformComponent() {};

		void OnParentChange(Entity* OldEntity, Entity* NewEntity)
		{
			CB_CORE_TRACE(
				"Parent for {0} changed from {1} to {2}!",
				GetEntity().GetName(),
				OldEntity ? OldEntity->GetName() : "nullptr",
				NewEntity ? NewEntity->GetName() : "nullptr"
			);

			MarkDirty(E_TRANSLATION);
			MarkDirty(E_ROTATION);
			MarkDirty(E_SCALING);
		}
		
		T m_Position = T(0.f);
		T m_Scale = T(1.f);

		bool m_IsTranslationDirty = false;
		bool m_IsRotationDirty = false;
		bool m_IsScaleDirty = false;
	};
}
