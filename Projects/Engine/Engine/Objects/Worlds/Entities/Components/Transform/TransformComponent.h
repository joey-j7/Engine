#pragma once

#include "Engine/Objects/Object.h"

#include "Engine/Events/Event.h"
#include "Transform.h"

#include <vector>

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
	
	template<class T = Vector3, class R = Vector3>
	class Engine_API TransformComponent : public Component
	{
		static_assert(std::is_same<T, Vector2>() || std::is_same<T, Vector3>(), "Illegal transform type detected");
		static_assert(std::is_same<R, float>() || std::is_same<R, Vector3>(), "Illegal rotation type detected");

	public:
		static const TransformDimension Dimension = static_cast<TransformDimension>(std::is_same<T, Vector3>() + 1);

		const Transform<T, R>& GetTransform(bool Local = true) const
		{
			return m_LocalTransform;
		}

		const T& GetPosition(bool Local = false)
		{
			if (Local)
			{
				return m_LocalTransform.m_Position;
			}

			if (m_TranslationDirty)
			{
				m_CachedGlobalTransform.m_Position = m_ParentTransform ?
					m_ParentTransform->GetPosition(false) + m_LocalTransform.m_Position :
					m_LocalTransform.m_Position
				;
				
				m_TranslationDirty = false;
			}

			return m_CachedGlobalTransform.m_Position;
		}
		
		void SetPosition(const T& ToPosition, bool Local = true)
		{
			if (
				(Local && ToPosition == m_LocalTransform.m_Position)
			)
				return;

			// Local -> Local can be set directly, global is delayed (avoids nested lookup)
			if (Local)
			{
				T& Old = m_LocalTransform.m_Position;

				Transform<T, R> From = Transform(Old, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);
				Transform<T, R> To = Transform(ToPosition, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);

				OnTranslation(Old, ToPosition);
				OnTransformation(From, To);

				Old = ToPosition;

				if (m_ParentTransform)
				{
					MarkDirty(E_TRANSLATION);
				}
				else
				{
					m_CachedGlobalTransform.m_Position = ToPosition;
					m_TranslationDirty = false;
				}
			}
			// Global -> Local will be set directly
			else
			{
				const T& OldGlobal = GetPosition();
				const T& NewGlobal = ToPosition;
				const T DiffGlobal = NewGlobal - OldGlobal;

				const T& OldLocal = m_LocalTransform.m_Position;
				const T NewLocal = m_LocalTransform.m_Position + DiffGlobal;

				Transform<T, R> From = Transform(OldLocal, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);
				Transform<T, R> To = Transform(NewLocal, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);

				OnTranslation(OldLocal, NewLocal);
				OnTransformation(From, To);

				m_LocalTransform.m_Position = NewLocal;
				m_CachedGlobalTransform.m_Position = NewGlobal;
			}
			
			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_TRANSLATION);
			}
		}
		
		void Translate(const T& Translation, bool Local = false)
		{
			if (Translation == T(0.f))
				return;

			T Old = m_LocalTransform.m_Position;
			T ToAdd = Translation;
			
			// TODO: Local should consider translation based on current direction
			if (Local)
			{
				
			}
			
			m_LocalTransform.m_Position += ToAdd;
			m_CachedGlobalTransform.m_Position += ToAdd;

			Transform<T, R> From = Transform<T, R>(Old, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);
			Transform<T, R> To = Transform<T, R>(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);

			OnTranslation(Old, m_LocalTransform.m_Position);
			OnTransformation(From, To);

			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_TRANSLATION);
			}
		}

		const R& GetRotation(bool Local = false)
		{
			if (Local)
			{
				return m_LocalTransform.m_Rotation;
			}

			if (m_RotationDirty)
			{
				m_CachedGlobalTransform.m_Rotation = m_ParentTransform ?
					m_ParentTransform->GetRotation(false) + m_LocalTransform.m_Rotation :
					m_LocalTransform.m_Rotation
				;
				
				m_RotationDirty = false;
			}

			return m_CachedGlobalTransform.m_Rotation;
		}
		
		void SetRotation(const R& ToRotation, bool Local = false)
		{
			if (
				(Local && ToRotation == m_LocalTransform.m_Rotation) ||
				(!Local && ToRotation == m_CachedGlobalTransform.m_Rotation)
			)
				return;

			// Local -> Local can be set directly, global is delayed (avoids nested lookup)
			if (Local)
			{
				R& Old = m_LocalTransform.m_Rotation;

				Transform<T, R> From = Transform(m_LocalTransform.m_Position, Old, m_LocalTransform.m_Scale);
				Transform<T, R> To = Transform(m_LocalTransform.m_Position, ToRotation, m_LocalTransform.m_Scale);

				OnRotation(Old, ToRotation);
				OnTransformation(From, To);

				Old = ToRotation;
				
				if (m_ParentTransform)
				{
					MarkDirty(E_ROTATION);
				}
				else
				{
					m_CachedGlobalTransform.m_Rotation = ToRotation;
					m_RotationDirty = false;
				}
			}
			// Global -> Local will be set directly
			else
			{
				const R& OldGlobal = m_CachedGlobalTransform.m_Rotation;
				const R& NewGlobal = ToRotation;
				const R DiffGlobal = NewGlobal - OldGlobal;

				const R& OldLocal = m_LocalTransform.m_Rotation;
				const R NewLocal = m_LocalTransform.m_Rotation + DiffGlobal;

				Transform<T, R> From = Transform(m_LocalTransform.m_Position, OldLocal, m_LocalTransform.m_Scale);
				Transform<T, R> To = Transform(m_LocalTransform.m_Position, NewLocal, m_LocalTransform.m_Scale);

				OnRotation(OldLocal, NewLocal);
				OnTransformation(From, To);

				m_LocalTransform.m_Rotation = NewLocal;
				m_CachedGlobalTransform.m_Rotation = NewGlobal;
			}

			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_ROTATION);
			}
		}
		
		void Rotate(const R& Rotation, bool Local = false)
		{
			if (Rotation == R(0.f))
				return;

			R Old = m_LocalTransform.m_Rotation;
			R ToAdd = Rotation;

			// TODO: Local should consider rotation based on current direction
			if (Local)
			{

			}

			m_LocalTransform.m_Rotation += ToAdd;
			m_CachedGlobalTransform.m_Rotation += ToAdd;

			Transform<T, R> From = Transform<T, R>(m_LocalTransform.m_Position, Old, m_LocalTransform.m_Scale);
			Transform<T, R> To = Transform<T, R>(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);

			OnRotation(Old, m_LocalTransform.m_Rotation);
			OnTransformation(From, To);

			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_ROTATION);
			}
		}
		
		const T& GetScale(bool Local = false)
		{
			if (Local)
			{
				return m_LocalTransform.m_Scale;
			}

			if (m_ScaleDirty)
			{
				m_CachedGlobalTransform.m_Scale = m_ParentTransform ?
					m_ParentTransform->GetScale(false) * m_LocalTransform.m_Scale :
					m_LocalTransform.m_Scale
				;
				
				m_ScaleDirty = false;
			}

			return m_CachedGlobalTransform.m_Scale;
		}
		
		void SetScale(const T& ToScale, bool Local = true)
		{
			if (
				(Local && ToScale == m_LocalTransform.m_Scale) ||
				(!Local && ToScale == m_CachedGlobalTransform.m_Scale)
			)
				return;

			// Local -> Local can be set directly, global is delayed (avoids nested lookup)
			if (Local)
			{
				T& Old = m_LocalTransform.m_Scale;

				Transform<T, R> From = Transform(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, Old);
				Transform<T, R> To = Transform(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, ToScale);

				OnScale(Old, ToScale);
				OnTransformation(From, To);

				Old = ToScale;

				if (m_ParentTransform)
				{
					MarkDirty(E_SCALING);
				}
				else
				{
					m_CachedGlobalTransform.m_Scale = ToScale;
					m_ScaleDirty = false;
				}
			}
			// Global -> Local will be set directly, TODO: fix
			else
			{
				const T& OldGlobal = m_CachedGlobalTransform.m_Scale;
				const T& NewGlobal = ToScale;
				const T DiffGlobal = NewGlobal - OldGlobal;

				const T& OldLocal = m_LocalTransform.m_Scale;
				const T NewLocal = m_LocalTransform.m_Scale + DiffGlobal;

				Transform<T, R> From = Transform(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, OldLocal);
				Transform<T, R> To = Transform(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, NewLocal);

				OnScale(OldLocal, NewLocal);
				OnTransformation(From, To);

				m_LocalTransform.m_Scale = NewLocal;
				m_CachedGlobalTransform.m_Scale = NewGlobal;
			}

			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_SCALING);
			}
		}
		
		void Scale(const T& Scaling, bool Local = false)
		{
			if (Scaling == T(1.f))
				return;

			T Old = m_LocalTransform.m_Scale;
			T ToAdd = Scaling;

			// TODO: Local should consider scaling based on current direction
			if (Local)
			{

			}

			m_LocalTransform.m_Scale *= ToAdd;
			m_CachedGlobalTransform.m_Scale *= ToAdd;

			Transform<T, R> From = Transform<T, R>(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, Old);
			Transform<T, R> To = Transform<T, R>(m_LocalTransform.m_Position, m_LocalTransform.m_Rotation, m_LocalTransform.m_Scale);

			OnScale(Old, m_LocalTransform.m_Scale);
			OnTransformation(From, To);

			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(E_SCALING);
			}
		}
		
		void MarkDirty(TransformType Type)
		{
			switch (Type)
			{
			case E_TRANSLATION:
				if (m_TranslationDirty)
					return;

				m_TranslationDirty = true;
				break;
			case E_ROTATION:
				if (m_RotationDirty)
					return;

				m_RotationDirty = true;
				break;
			case E_SCALING:
			default:
				if (m_ScaleDirty)
					return;

				m_ScaleDirty = true;
				break;
			}
			
			// Mark all children as dirty
			for (auto Transform : m_ChildTransforms)
			{
				Transform->MarkDirty(Type);
			}
		}

		bool IsDirty(TransformType Type) const
		{
			switch (Type)
			{
			case E_TRANSLATION:
				return m_TranslationDirty;
			case E_ROTATION:
				return m_RotationDirty;
			case E_SCALING:
				return m_ScaleDirty;
			default:
				return m_TranslationDirty || m_RotationDirty || m_ScaleDirty;
			}
		}

		Event<void, const T&, const T&> OnTranslation = Event<void, const T&, const T&>("TransformComponent::OnTranslation");
		Event<void, const R&, const R&> OnRotation = Event<void, const R&, const R&>("TransformComponent::OnRotation");
		Event<void, const T&, const T&> OnScale = Event<void, const T&, const T&>("TransformComponent::OnScale");
		Event<void, const Transform<T, R>&, const Transform<T, R>&> OnTransformation = Event<void, const Transform<T, R>&, const Transform<T, R>&>("TransformComponent::OnTransformation");
		
	protected:
		Event<void, Entity&, Entity*, Entity*>::HandlerType::IDType m_ParentChangedID;
		
		TransformComponent(Entity& Entity, const String& sName = "Transform Component") : Component(Entity, sName)
		{
			m_ForceUniqueness = true;

			m_ParentChangedID = GetEntity().OnParentChanged.Bind(
				this,
				&TransformComponent<T, R>::OnParentChanged
			);

			AssignParentTransform();
			AssignChildTransforms();
		}

		virtual ~TransformComponent()
		{
			GetEntity().OnParentChanged.Unbind(m_ParentChangedID);
		}

		void AssignParentTransform()
		{
			// Get first compatible TransformComponent parent
			Entity* Parent = GetEntity().GetParent();

			while (Parent)
			{
				auto Components = Parent->template GetComponentsOfType<TransformComponent<T, R>>();

				if (Components.size() > 0)
				{
					// Still the same
					if (m_ParentTransform == Components.at(0))
						break;
					
					m_ParentTransform = Components.at(0);
					
					MarkDirty(E_TRANSLATION);
					MarkDirty(E_ROTATION);
					MarkDirty(E_SCALING);
					
					break;
				}

				Parent = Parent->GetParent();
			}
		}

		void AssignChildTransforms()
		{
			m_ChildTransforms.clear();
			
			for (Entity* Child : GetEntity().GetChildren())
			{
				auto Components = Child->GetComponentsOfType<TransformComponent<T, R>>();

				for (TransformComponent<T, R>* Component : Components)
				{
					m_ChildTransforms.push_back(Component);
				}
			}
		}
		
		void OnParentChanged(Entity& Origin, Entity* OldEntity, Entity* NewEntity)
		{
			// Unassign its transforms
			if (&Origin == &GetEntity() && OldEntity)
			{
				auto Components = OldEntity->GetComponentsOfType<TransformComponent<T, R>>();

				for (TransformComponent<T, R>* Component : Components)
				{
					m_ChildTransforms.erase(
						std::find(
							m_ChildTransforms.begin(),
							m_ChildTransforms.end(),
							Component
						)
					);
				}
			}

			// Assign its transforms
			if (&Origin == &GetEntity() && NewEntity)
			{
				auto Components = NewEntity->GetComponentsOfType<TransformComponent<T, R>>();

				for (TransformComponent<T, R>* Component : Components)
				{
					const auto Find = std::find(
						Component->m_ChildTransforms.begin(),
						Component->m_ChildTransforms.end(),
						this
					);

					if (Find == Component->m_ChildTransforms.end())
						Component->m_ChildTransforms.push_back(this);
				}
			}

			// Check closest parent transform, old parent might not be valid due to chain break
			// This component will be marked dirty if any parent change has occurred
			AssignParentTransform();
		}
		
		Transform<T, R> m_LocalTransform;
		Transform<T, R> m_CachedGlobalTransform;
		
		// First parent that holds a valid transform
		TransformComponent<T, R>* m_ParentTransform = nullptr;
		std::vector<TransformComponent<T, R>*> m_ChildTransforms;
		
		// For performance considerations, some transformations are not instantly applied
		// but should be updated in the future (in getter) during the following scenarios:
		// On LOCAL --> Local directly, global delayed
		// On GLOBAL-- > Local directly, global directly
		bool m_TranslationDirty = true;
		bool m_RotationDirty = true;
		bool m_ScaleDirty = true;
	};
}
