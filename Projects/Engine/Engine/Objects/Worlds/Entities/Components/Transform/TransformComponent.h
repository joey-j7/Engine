#pragma once

#include "Engine/Objects/Object.h"
#include "Engine/Objects/Worlds/Entities/Components/Component.h"

namespace Engine
{
	template<class T = glm::vec3>
	class Engine_API TransformComponent : public Component
	{
	public:
		const T& GetPosition() const;
		void SetPosition(const T& ToPosition);
		void Translate(const T& Translation);
		
		const T& GetScale() const;
		void SetScale(const T& ToScale);
		void Scale(const T& Scaling, bool Local = false);

		void MarkDirty() { MarkDirty(); }
		
	protected:
		TransformComponent(Entity& Entity, const std::string& sName = "Transform Component");
		virtual ~TransformComponent() {};

		void OnParentChange(Entity* OldEntity, Entity* NewEntity);
		
		T m_Position = T(0.f);
		T m_Scale = T(1.f);

		bool m_IsDirty = false;
	};

	template <class T>
	const T& TransformComponent<T>::GetPosition() const
	{
		return m_Position;
	}

	template <class T>
	void TransformComponent<T>::SetPosition(const T& ToPosition)
	{
		if (ToPosition == m_Position)
			return;
		
		m_Position = ToPosition;
		MarkDirty();
	}

	template <class T>
	void TransformComponent<T>::Translate(const T& Translation)
	{
		if (Translation == T(0.f))
			return;

		m_Position += Translation;
		MarkDirty();
	}

	template <class T>
	const T& TransformComponent<T>::GetScale() const
	{
		return m_Scale;
	}

	template <class T>
	void TransformComponent<T>::SetScale(const T& ToScale)
	{
		if (ToScale == m_Scale)
			return;
		
		m_Scale = ToScale;
		MarkDirty();
	}

	template <class T>
	void TransformComponent<T>::Scale(const T& Scaling, bool Local)
	{
		if (
			(m_Scale == T(1.f) && Local) ||
			(m_Scale == T(0.f) && !Local)
		)
			return;
		
		if (Local)
			m_Scale *= Scaling;
		else
			m_Scale += Scaling;

		MarkDirty();
	}

	template <class T>
	TransformComponent<T>::TransformComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName)
	{
		GetEntity().OnParentChange.Bind(
			this,
			&TransformComponent<T>::OnParentChange
		);
	}

	template <class T>
	void TransformComponent<T>::OnParentChange(Entity* OldEntity, Entity* NewEntity)
	{
		CB_CORE_TRACE(
			"Parent for {0} changed from {1} to {2}!",
			GetEntity().GetName(),
			OldEntity ? OldEntity->GetName() : "nullptr",
			NewEntity ? NewEntity->GetName() : "nullptr"
		);
		
		MarkDirty();
	}
}
