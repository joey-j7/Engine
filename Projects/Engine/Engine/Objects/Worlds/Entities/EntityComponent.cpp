#include "pch.h"
#include "EntityComponent.h"

#include "Components/RenderComponent.h"
#include "Components/UI/Layouts/UILayout.h"

#include "Components/Transform/Transform3DComponent.h"
#include "Components/Transform/Transform2DComponent.h"

namespace Engine
{
	Entity::Entity()
	{
		m_pWorld = Application::Get().GetWorldManager().GetActive();
		m_pWorld->m_pWorldObjects.push_back(this);
	}

	Entity::~Entity()
	{
		// Pass along parent to children
		while (!m_Children.empty())
		{
			m_Children.front()->SetParent(GetParent());
		}
		
		SetParent(nullptr);

		m_pWorld->m_pWorldObjects.erase(
			std::find(
				m_pWorld->m_pWorldObjects.begin(),
				m_pWorld->m_pWorldObjects.end(),
				this
			)
		);
	}

	const AABB Entity::GetBounds() const
	{
		AABB Bounds;

		Bounds.fLeft = FLT_MAX;
		Bounds.fTop = FLT_MAX;
		Bounds.fRight = FLT_MIN;
		Bounds.fBottom = FLT_MIN;

		bool Found = false;

		auto Render3DComponents = GetComponentsOfType<Render3DComponent>();

		for (auto& Component : Render3DComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, Bounds2.fLeft);
			Bounds.fTop = glm::min(Bounds.fTop, Bounds2.fTop);
			Bounds.fRight = glm::max(Bounds.fRight, Bounds2.fRight);
			Bounds.fBottom = glm::max(Bounds.fBottom, Bounds2.fBottom);

			Found = true;
		}

		auto Render2DComponents = GetComponentsOfType<Render2DComponent>();

		for (auto& Component : Render2DComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, Bounds2.fLeft);
			Bounds.fTop = glm::min(Bounds.fTop, Bounds2.fTop);
			Bounds.fRight = glm::max(Bounds.fRight, Bounds2.fRight);
			Bounds.fBottom = glm::max(Bounds.fBottom, Bounds2.fBottom);

			Found = true;
		}

		auto LayoutComponents = GetComponentsOfType<UILayout>();

		for (auto& Component : LayoutComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, Bounds2.fLeft);
			Bounds.fTop = glm::min(Bounds.fTop, Bounds2.fTop);
			Bounds.fRight = glm::max(Bounds.fRight, Bounds2.fRight);
			Bounds.fBottom = glm::max(Bounds.fBottom, Bounds2.fBottom);

			Found = true;
		}

		if (!Found)
		{
			Bounds.fLeft = Bounds.fTop = Bounds.fRight = Bounds.fBottom = 0.f;
		}

		return Bounds;
	}

	const SkMatrix Entity::GetMatrix() const
	{
		SkMatrix Mat = SkMatrix();
		
		auto Component3D = GetComponentsOfType<Transform3DComponent>();

		if (!Component3D.empty())
		{
			const Vector3& Position = Component3D[0]->GetPosition(false);
			const Vector3& Rotation = Component3D[0]->GetRotation(false);
			const Vector3& Scale = Component3D[0]->GetScale(false);

			// TODO: Create 3D matrix
			Mat = SkMatrix::Concat(Mat, SkMatrix::Scale(Scale.x, Scale.y));
			//Mat = SkMatrix::Concat(Mat, SkMatrix::RotateDeg(Rotation));
			Mat = SkMatrix::Concat(Mat, SkMatrix::Translate(Position.x, Position.y));
		}
		
		auto Component2D = GetComponentsOfType<Transform2DComponent>();

		if (!Component2D.empty())
		{
			const Vector2& Position = Component2D[0]->GetPosition(false);
			float Rotation = Component2D[0]->GetRotation(false);
			const Vector2& Scale = Component2D[0]->GetScale(false);

			Mat = SkMatrix::Concat(Mat, SkMatrix::Scale(Scale.x, Scale.y));
			Mat = SkMatrix::Concat(Mat, SkMatrix::RotateDeg(Rotation));
			Mat = SkMatrix::Concat(Mat, SkMatrix::Translate(Position.x, Position.y));
		}

		if (GetParent())
		{
			Mat = SkMatrix::Concat(Mat, GetParent()->GetMatrix());
		}
		
		return Mat;
	}

	bool Entity::RemoveComponent(Component* Comp)
	{
		uint32_t ID = Comp->GetID();
		
		auto Find = m_Components.find(ID);
		const bool Found = Find != m_Components.end();
		
		if (Found)
		{
			OnComponentRemoved(*this, *Find->second);
			CB_CORE_TRACE("Removed component {0} from {1}", Comp->GetName(), GetName());

			m_Components.erase(Find);
			
			// Remove all components that depend on this
			for (auto& Component : m_Components)
			{
				if (!Component.second->HasDependency(ID))
					continue;

				RemoveComponent(Component.second.get());
			}
		}
		
		return Found;
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (m_Parent == NewEntity)
			return;
		
		if (m_Parent)
		{
			m_Parent->OnChildRemoved(*m_Parent, *this);
			m_Parent->m_Children.erase(std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this));
		}

		Entity* OldEntity = m_Parent;
		m_Parent = NewEntity;
		
		OnParentChanged(*this, OldEntity, NewEntity);
		NotifyChildrenOnParentChanged(*this, OldEntity, NewEntity);

		/*const String OldName = (OldEntity ? OldEntity->GetName() : "nullptr");
		const String NewName = (NewEntity ? NewEntity->GetName() : "nullptr");

		CB_CORE_TRACE(
			"Parent for {0} changed from {1} to {2}",
			GetName(),
			OldName,
			NewName
		);*/

		if (!m_Parent)
			return;

		m_Parent->m_Children.push_back(this);
		m_Parent->OnChildAdded(*m_Parent, *this);
	}

	void Entity::NotifyChildrenOnParentChanged(Entity& Origin, Entity* Old, Entity* New)
	{
		// Notify all children that this entity's parent has changed, nested loop
		for (Entity* Child : m_Children)
		{
			Child->OnParentChanged(Origin, Old, New);
			Child->NotifyChildrenOnParentChanged(Origin, Old, New);
		}
	}
}
