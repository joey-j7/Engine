#pragma once

#include "Engine/Objects/Object.h"

#include "Engine/Objects/Worlds/Entities/Entity.h"
#include "Engine/Objects/Worlds/Entities/Components/Component.h"

#include "Engine/General/Math.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	enum TransformType
	{
		E_TRANSLATION = 0,
		E_ROTATION = 1,
		E_SCALING = 2
	};

	enum TransformDimension
	{
		E_NONE = 0,
		E_2D = 1,
		E_3D = 2
	};

	template <typename T = Vector3, typename R = Vector3>
	struct Transform
	{
		static_assert(std::is_same<T, Vector2>() || std::is_same<T, Vector3>(), "Illegal transform type detected");
		static_assert(std::is_same<R, float>() || std::is_same<R, Vector3>(), "Illegal rotation type detected");

		Transform() {}
		Transform(const T& Position, const R& Rotation, const T& Scale) : m_Position(Position),
			m_Rotation(Rotation),
			m_Scale(Scale)
		{}

		T m_Position = T(0.f);
		R m_Rotation = R(0.f);
		T m_Scale = T(1.f);
	};
	
	template<class T = Vector3, class R = Vector3>
	class Engine_API TransformComponent : public Component
	{
		static_assert(std::is_same<T, Vector2>() || std::is_same<T, Vector3>(), "Illegal transform type detected");
		static_assert(std::is_same<R, float>() || std::is_same<R, Vector3>(), "Illegal rotation type detected");

	public:
		static const TransformDimension Dimension = static_cast<TransformDimension>(std::is_same<T, Vector3>() + 1);

		const Transform<T, R>& GetTransform(bool Local = true) const
		{
			return m_Transform;
		}

		const T& GetPosition(bool Local = true) const
		{
			return m_Transform.m_Position;
		}
		
		void SetPosition(const T& ToPosition, bool Local = true)
		{
			if (ToPosition == m_Transform.m_Position)
				return;

			MarkDirty(E_TRANSLATION);

			T Old = m_Transform.m_Position;
			
			Transform<T, R> From = Transform(Old, m_Transform.m_Rotation, m_Transform.m_Scale);
			Transform<T, R> To = Transform(ToPosition, m_Transform.m_Rotation, m_Transform.m_Scale);
			
			if (Local)
			{
				m_Transform.m_Position = ToPosition;
				
				OnTranslation(Old, ToPosition);
				OnTransformation(From, To);

				return;
			}

			// TODO: Global
			m_Transform.m_Position = ToPosition;

			OnTranslation(Old, ToPosition);
			OnTransformation(From, To);
		}
		
		void Translate(const T& Translation)
		{
			if (Translation == T(0.f))
				return;

			T Old = m_Transform.m_Position;

			m_Transform.m_Position += Translation;

			Transform<T, R> From = Transform<T, R>(Old, m_Transform.m_Rotation, m_Transform.m_Scale);
			Transform<T, R> To = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, m_Transform.m_Scale);

			OnTranslation(Old, m_Transform.m_Position);
			OnTransformation(From, To);
			
			MarkDirty(E_TRANSLATION);
		}

		const R& GetRotation(bool Local = true) const
		{
			if (Local)
			{
				return m_Transform.m_Rotation;
			}

			// TODO: Global
			return m_Transform.m_Rotation;
		}
		
		void SetRotation(const R& Rotation, bool Local = true)
		{
			if (m_Transform.m_Rotation == Rotation)
				return;

			MarkDirty(E_ROTATION);

			R Old = Rotation;
			
			Transform<T, R> From = Transform<T, R>(m_Transform.m_Position, Old, m_Transform.m_Scale);
			Transform<T, R> To = Transform<T, R>(m_Transform.m_Position, Rotation, m_Transform.m_Scale);

			if (Local)
			{
				m_Transform.m_Rotation = Rotation;

				OnRotation(Old, m_Transform.m_Rotation);
				OnTransformation(From, To);
				
				return;
			}

			// TODO: Global
			m_Transform.m_Rotation = Rotation;

			OnRotation(Old, m_Transform.m_Rotation);
			OnTransformation(From, To);
		}
		
		void Rotate(const R& Rotation)
		{
			if (Rotation == R(0.f))
				return;

			R Old = Rotation;

			m_Transform.m_Rotation += Rotation;

			Transform<T, R> From = Transform<T, R>(m_Transform.m_Position, Old, m_Transform.m_Scale);
			Transform<T, R> To = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, m_Transform.m_Scale);

			OnRotation(Old, m_Transform.m_Rotation);
			OnTransformation(From, To);
			
			MarkDirty(E_ROTATION);
		}
		
		const T& GetScale(bool Local = true) const
		{
			if (Local)
			{
				return m_Transform.m_Scale;
			}

			// TODO: Global
			return m_Transform.m_Scale;
		}
		
		void SetScale(const T& ToScale, bool Local = true)
		{
			if (ToScale == m_Transform.m_Scale)
				return;

			MarkDirty(E_SCALING);

			T Old = m_Transform.m_Scale;
			
			Transform<T, R> From = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, Old);
			Transform<T, R> To = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, ToScale);

			if (Local)
			{
				m_Transform.m_Scale = ToScale;

				OnScale(Old, m_Transform.m_Scale);
				OnTransformation(From, To);
				
				return;
			}

			// TODO: Global
			m_Transform.m_Scale = ToScale;

			OnScale(Old, m_Transform.m_Scale);
			OnTransformation(From, To);
		}
		
		void Scale(const T& Scaling)
		{
			if (m_Transform.m_Scale == T(1.f))
				return;

			MarkDirty(E_SCALING);
			
			T Old = m_Transform.m_Scale;
			m_Transform.m_Scale *= Scaling;

			Transform<T, R> From = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, Old);
			Transform<T, R> To = Transform<T, R>(m_Transform.m_Position, m_Transform.m_Rotation, m_Transform.m_Scale);

			OnScale(Old, m_Transform.m_Scale);
			OnTransformation(From, To);
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
				return m_IsScaleDirty;
			default:
				return m_IsTranslationDirty;
			}
		}

		Event<void, const T&, const T&> OnTranslation = Event<void, const T&, const T&>("TransformComponent::OnTranslation");
		Event<void, const R&, const R&> OnRotation = Event<void, const R&, const R&>("TransformComponent::OnRotation");
		Event<void, const T&, const T&> OnScale = Event<void, const T&, const T&>("TransformComponent::OnScale");
		Event<void, const Transform<T, R>&, const Transform<T, R>&> OnTransformation = Event<void, const Transform<T, R>&, const Transform<T, R>&>("TransformComponent::OnTransformation");
		
	protected:
		TransformComponent(Entity& Entity, const std::string& sName = "Transform Component") : Component(Entity, sName)
		{
			m_ForceUniqueness = true;

			GetEntity().OnParentChanged.Bind(
				this,
				&TransformComponent<T, R>::OnParentChanged
			);
		}

		virtual ~TransformComponent() {};
		
		void OnParentChanged(Entity* OldEntity, Entity* NewEntity)
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

		Transform<T, R> m_Transform;

		bool m_IsTranslationDirty = false;
		bool m_IsRotationDirty = false;
		bool m_IsScaleDirty = false;
	};

	template TransformComponent<Vector3, Vector3>;
	template TransformComponent<Vector2, float>;
}
